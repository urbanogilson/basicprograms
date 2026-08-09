//! The esp-hal I2S output driver.
//!
//! ```no_run
//! # fn demo(peripherals: esp_hal::peripherals::Peripherals) -> Result<(), i2s_speaker::Error> {
//! use esp_hal::dma_buffers;
//! use esp_hal::time::Rate;
//! use i2s_speaker::{Max98357a, Wav};
//!
//! static SOUND: &[u8] = include_bytes!("../../../firmware/assets/sound.wav");
//! let wav = Wav::parse(SOUND).unwrap();
//!
//! let (_, _, tx_buffer, tx_descriptors) = dma_buffers!(0, 8 * 4092);
//!
//! let mut speaker = Max98357a::builder(peripherals.I2S0, peripherals.DMA_CH0)
//!     .with_pins(peripherals.GPIO15, peripherals.GPIO16, peripherals.GPIO7)
//!     .with_sample_rate(Rate::from_hz(wav.sample_rate()))
//!     .build(tx_descriptors, tx_buffer)?;
//!
//! let mut playback = speaker.start()?;
//! let mut samples = wav.samples();
//! let mut pending: Option<i16> = None;
//! loop {
//!     // ... feed `playback.write(..)` from `samples` until it runs out
//!     # let _ = (&mut samples, &mut pending);
//!     # break;
//! }
//! playback.flush()?;
//! # Ok(())
//! # }
//! ```

use core::marker::PhantomData;

use esp_hal::Blocking;
use esp_hal::dma::{DmaChannelFor, DmaDescriptor, DmaError, DmaTransferTxCircular};
use esp_hal::gpio::interconnect::PeripheralOutput;
use esp_hal::i2s::AnyI2s;
use esp_hal::i2s::master::{
    Channels, ConfigError, DataFormat, Error as I2sError, I2s, I2sTx, Instance,
};
use esp_hal::time::Rate;

use crate::part::AmpPart;

/// Sample rate used unless [`SpeakerBuilder::with_sample_rate`] says otherwise.
pub const DEFAULT_SAMPLE_RATE: Rate = Rate::from_hz(16_000);

/// Frames staged per `push`. Small because, unlike a DMA read, a write may take less than the
/// space available - so there is no need to absorb everything at once. Sized so the staging
/// buffer stays under the crate's 1024-byte `large_stack_frames` threshold even with 32-bit slots.
const STAGE_FRAMES: usize = 64;

/// Largest stereo frame we emit: two 32-bit slots.
const MAX_FRAME_BYTES: usize = 8;

/// How an `i16` sample is laid out in the wire format.
#[derive(Clone, Copy, Debug)]
struct SlotLayout {
    /// Bytes per slot: 2 for 16-bit slots, 4 for 32-bit.
    slot_bytes: usize,
    /// How far left to shift the sample so it lands in the slot's most significant bits.
    shift: u32,
}

impl SlotLayout {
    fn of(format: DataFormat) -> Self {
        Self {
            slot_bytes: (format.channel_bits() / 8) as usize,
            // I2S is MSB-first, so a 16-bit sample in a wider slot must be left-justified or it
            // plays as near-silence.
            shift: (format.data_bits() as u32).saturating_sub(16),
        }
    }

    fn frame_bytes(&self) -> usize {
        self.slot_bytes * 2
    }

    /// Write one sample into both slots of `out`, which must be `frame_bytes()` long.
    fn write_frame(&self, sample: i16, out: &mut [u8]) {
        let value = (sample as i32) << self.shift;
        let bytes = value.to_le_bytes();
        // A 16-bit slot takes the low half; a 32-bit slot takes all four bytes.
        let slot = &bytes[..self.slot_bytes];
        out[..self.slot_bytes].copy_from_slice(slot);
        out[self.slot_bytes..self.slot_bytes * 2].copy_from_slice(slot);
    }
}

/// Something went wrong setting up or feeding the amplifier.
#[derive(Clone, Copy, Debug, PartialEq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Error {
    /// The I2S peripheral rejected the configuration.
    Config(ConfigError),
    /// The DMA rejected the transfer.
    Dma(DmaError),
    /// The I2S peripheral refused to start or run the transfer.
    I2s(I2sError),
    /// The amplifier does not lock to this sample rate. See the part's documentation for the
    /// rates it accepts - an out-of-band rate plays as noise rather than failing cleanly, so it
    /// is rejected here instead.
    UnsupportedSampleRate(u32),
}

impl From<ConfigError> for Error {
    fn from(e: ConfigError) -> Self {
        Error::Config(e)
    }
}

impl From<DmaError> for Error {
    fn from(e: DmaError) -> Self {
        Error::Dma(e)
    }
}

impl From<I2sError> for Error {
    fn from(e: I2sError) -> Self {
        match e {
            I2sError::DmaError(d) => Error::Dma(d),
            other => Error::I2s(other),
        }
    }
}

/// A configured I2S audio output.
///
/// Build one with [`I2sOut::builder`], or through a part alias such as [`crate::Max98357a`].
pub struct I2sOut<'d, P: AmpPart> {
    tx: I2sTx<'d, Blocking>,
    buffer: &'static mut [u8],
    sample_rate: Rate,
    layout: SlotLayout,
    _part: PhantomData<P>,
}

impl<'d, P: AmpPart> I2sOut<'d, P> {
    /// Start configuring an output on the given I2S peripheral and DMA channel.
    ///
    /// Pins are still required: [`SpeakerBuilder::build`] only exists once
    /// [`SpeakerBuilder::with_pins`] has supplied them.
    pub fn builder<I, C>(i2s: I, channel: C) -> SpeakerBuilder<'d, P, I, C> {
        SpeakerBuilder {
            i2s,
            channel,
            bclk: (),
            ws: (),
            dout: (),
            sample_rate: DEFAULT_SAMPLE_RATE,
            data_format: P::DATA_FORMAT,
            _part: PhantomData,
            _lifetime: PhantomData,
        }
    }

    /// Begin playback.
    ///
    /// The transfer is circular, so the ring is replayed if it is not kept fed. Feed it with
    /// [`Playback::write`] and end with [`Playback::flush`].
    pub fn start(&mut self) -> Result<Playback<'_, 'd, P>, Error> {
        let capacity = self.buffer.len();
        let transfer = self.tx.write_dma_circular(&self.buffer)?;
        Ok(Playback {
            transfer,
            capacity,
            layout: self.layout,
            _part: PhantomData,
        })
    }

    /// The configured sample rate.
    pub fn sample_rate(&self) -> Rate {
        self.sample_rate
    }
}

/// Builder returned by [`I2sOut::builder`].
///
/// The pin type parameters start as `()` and are replaced by [`SpeakerBuilder::with_pins`];
/// `build` is only implemented for real pin types, so forgetting the pins is a compile error.
pub struct SpeakerBuilder<'d, P, I, C, B = (), W = (), D = ()> {
    i2s: I,
    channel: C,
    bclk: B,
    ws: W,
    dout: D,
    sample_rate: Rate,
    data_format: DataFormat,
    _part: PhantomData<P>,
    _lifetime: PhantomData<&'d ()>,
}

impl<'d, P: AmpPart, I, C, B, W, D> SpeakerBuilder<'d, P, I, C, B, W, D> {
    /// Supply the three I2S pins: bit clock, word select, and data out to the amplifier.
    ///
    /// On a MAX98357A these are `BCLK`, `LRC` and `DIN`. There is no MCLK.
    pub fn with_pins<B2, W2, D2>(
        self,
        bclk: B2,
        ws: W2,
        dout: D2,
    ) -> SpeakerBuilder<'d, P, I, C, B2, W2, D2> {
        SpeakerBuilder {
            i2s: self.i2s,
            channel: self.channel,
            bclk,
            ws,
            dout,
            sample_rate: self.sample_rate,
            data_format: self.data_format,
            _part: PhantomData,
            _lifetime: PhantomData,
        }
    }

    /// Override [`DEFAULT_SAMPLE_RATE`]. Pass the rate the audio was recorded at, or it will play
    /// back at the wrong pitch.
    pub fn with_sample_rate(mut self, rate: Rate) -> Self {
        self.sample_rate = rate;
        self
    }

    /// Override the part's default slot format.
    ///
    /// Amplifiers differ in which BCLK/LRCLK ratios they lock to, and the ratio is set by the
    /// slot width: 16-bit slots give 32x, 32-bit slots give 64x. Samples are left-justified into
    /// wider slots automatically.
    pub fn with_data_format(mut self, data_format: DataFormat) -> Self {
        self.data_format = data_format;
        self
    }
}

impl<'d, P, I, C, B, W, D> SpeakerBuilder<'d, P, I, C, B, W, D>
where
    P: AmpPart,
    I: Instance + 'd,
    C: DmaChannelFor<AnyI2s<'d>>,
    B: PeripheralOutput<'d>,
    W: PeripheralOutput<'d>,
    D: PeripheralOutput<'d>,
{
    /// Configure the peripheral and take ownership of the DMA buffers.
    ///
    /// `buffer` is the circular DMA ring and sets how long playback can survive without being
    /// fed; at 16 kHz each 8 KB is about 128 ms of audio.
    pub fn build(
        self,
        descriptors: &'static mut [DmaDescriptor],
        buffer: &'static mut [u8],
    ) -> Result<I2sOut<'d, P>, Error> {
        if !P::supports_sample_rate(self.sample_rate.as_hz()) {
            return Err(Error::UnsupportedSampleRate(self.sample_rate.as_hz()));
        }

        let config = P::base_config()
            .with_sample_rate(self.sample_rate)
            .with_data_format(self.data_format)
            .with_channels(Channels::STEREO);

        let i2s = I2s::new(self.i2s, self.channel, config)?;
        let tx = i2s
            .i2s_tx
            .with_bclk(self.bclk)
            .with_ws(self.ws)
            .with_dout(self.dout)
            .build(descriptors);

        Ok(I2sOut {
            tx,
            buffer,
            sample_rate: self.sample_rate,
            layout: SlotLayout::of(self.data_format),
            _part: PhantomData,
        })
    }
}

/// An in-progress playback, produced by [`I2sOut::start`].
pub struct Playback<'a, 'd, P: AmpPart> {
    transfer: DmaTransferTxCircular<'a, I2sTx<'d, Blocking>>,
    capacity: usize,
    layout: SlotLayout,
    _part: PhantomData<P>,
}

impl<P: AmpPart> Playback<'_, '_, P> {
    /// Queue up to `samples.len()` mono samples, returning how many were accepted.
    ///
    /// Each sample is written to both slots, so it plays correctly whichever channel the
    /// amplifier is strapped to. Never blocks: `Ok(0)` means the ring is full, which is ordinary
    /// backpressure — call again once the DMA has drained some.
    pub fn write(&mut self, samples: &[i16]) -> Result<usize, Error> {
        let frame_bytes = self.layout.frame_bytes();
        let free_frames = self.transfer.available()? / frame_bytes;
        let total = free_frames.min(samples.len());
        let mut written = 0;

        // A push may be smaller than the space available (unlike a DMA read, which must take
        // everything buffered), so staging in small chunks is fine.
        let mut stage = [0u8; STAGE_FRAMES * MAX_FRAME_BYTES];
        while written < total {
            let n = (total - written).min(STAGE_FRAMES);
            for (i, &s) in samples[written..written + n].iter().enumerate() {
                let at = i * frame_bytes;
                self.layout.write_frame(s, &mut stage[at..at + frame_bytes]);
            }
            self.transfer.push(&stage[..n * frame_bytes])?;
            written += n;
        }

        Ok(written)
    }

    /// Whether the ring has drained completely, i.e. the DMA has run out of real audio and is
    /// replaying stale samples. Feed [`write`](Self::write) more often if this goes true.
    pub fn underrun(&mut self) -> Result<bool, Error> {
        Ok(self.transfer.available()? >= self.capacity.saturating_sub(self.layout.frame_bytes()))
    }

    /// Push silence through the ring and stop.
    ///
    /// Circular DMA replays whatever is left in the buffer, so stopping without flushing repeats
    /// a fragment of the tail as a glitch.
    pub fn flush(mut self) -> Result<(), Error> {
        let mut remaining = self.capacity / self.layout.frame_bytes();
        let silence = [0i16; STAGE_FRAMES];
        while remaining > 0 {
            let n = self.write(&silence[..remaining.min(silence.len())])?;
            remaining -= n;
        }
        self.transfer.stop()?;
        Ok(())
    }
}
