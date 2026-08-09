//! The esp-hal I2S driver.
//!
//! ```no_run
//! # fn demo(peripherals: esp_hal::peripherals::Peripherals) -> Result<(), i2s_mic::Error> {
//! use esp_hal::dma_buffers;
//! use esp_hal::time::Rate;
//! use i2s_mic::{DcBlocker, Inmp441};
//!
//! // The macro's second buffer is reused as the driver's scratch space.
//! let (rx_buffer, rx_descriptors, scratch, _) =
//!     dma_buffers!(8 * 4092, i2s_mic::DEFAULT_SCRATCH_BYTES);
//!
//! let mut mic = Inmp441::builder(peripherals.I2S0, peripherals.DMA_CH0)
//!     .with_pins(peripherals.GPIO4, peripherals.GPIO5, peripherals.GPIO6)
//!     .with_sample_rate(Rate::from_hz(16_000))
//!     .build(rx_descriptors, rx_buffer, scratch)?;
//!
//! let mut dc = DcBlocker::new();
//! let mut samples = [0i16; 512];
//!
//! let mut recording = mic.start()?;
//! recording.discard(16_000)?; // let the microphone settle
//! loop {
//!     let n = recording.read(&mut samples)?;
//!     dc.process(&mut samples[..n]);
//!     // ... do something with &samples[..n]
//! }
//! # }
//! ```

use core::marker::PhantomData;

use esp_hal::Blocking;
use esp_hal::dma::{DmaChannelFor, DmaDescriptor, DmaError, DmaTransferRxCircular};
use esp_hal::gpio::interconnect::{PeripheralInput, PeripheralOutput};
use esp_hal::i2s::AnyI2s;
use esp_hal::i2s::master::{
    Channels, ConfigError, Error as I2sError, I2s, I2sRx, Instance,
};
use esp_hal::time::Rate;

use crate::format::{FRAME_BYTES, Slot, unpack_mono};
use crate::part::MicPart;

/// Sample rate used unless [`MicBuilder::with_sample_rate`] says otherwise.
pub const DEFAULT_SAMPLE_RATE: Rate = Rate::from_hz(16_000);

/// A reasonable scratch size for [`MicBuilder::build`]: 4096 bytes is 32 ms of slack at 16 kHz.
pub const DEFAULT_SCRATCH_BYTES: usize = 4096;

/// Something went wrong setting up or reading the microphone.
#[derive(Clone, Copy, Debug, PartialEq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Error {
    /// The I2S peripheral rejected the configuration.
    Config(ConfigError),
    /// The DMA buffer overran and samples were lost. Drop the [`Recording`] and call
    /// [`I2sMic::start`] again to resynchronise.
    Overrun(DmaError),
    /// The I2S peripheral refused to start or run the transfer.
    I2s(I2sError),
}

impl From<ConfigError> for Error {
    fn from(e: ConfigError) -> Self {
        Error::Config(e)
    }
}

impl From<DmaError> for Error {
    fn from(e: DmaError) -> Self {
        Error::Overrun(e)
    }
}

impl From<I2sError> for Error {
    fn from(e: I2sError) -> Self {
        // A DMA fault during a circular read means exactly one thing: we fell behind.
        match e {
            I2sError::DmaError(d) => Error::Overrun(d),
            other => Error::I2s(other),
        }
    }
}

/// A configured I2S microphone.
///
/// Build one with [`I2sMic::builder`], or through a part alias such as [`crate::Inmp441`].
pub struct I2sMic<'d, P: MicPart> {
    rx: I2sRx<'d, Blocking>,
    buffer: &'static mut [u8],
    scratch: &'static mut [u8],
    slot: Slot,
    sample_rate: Rate,
    _part: PhantomData<P>,
}

impl<'d, P: MicPart> I2sMic<'d, P> {
    /// Start configuring a microphone on the given I2S peripheral and DMA channel.
    ///
    /// Pins are still required: [`MicBuilder::build`] only exists once
    /// [`MicBuilder::with_pins`] has supplied them.
    pub fn builder<I, C>(i2s: I, channel: C) -> MicBuilder<'d, P, I, C> {
        MicBuilder {
            i2s,
            channel,
            bclk: (),
            ws: (),
            din: (),
            sample_rate: DEFAULT_SAMPLE_RATE,
            slot: P::DEFAULT_SLOT,
            _part: PhantomData,
            _lifetime: PhantomData,
        }
    }

    /// Begin capturing.
    ///
    /// Capture is circular DMA, which keeps the bit clock running continuously — MEMS parts like
    /// the INMP441 require that. Calling this again after an overrun restarts the transfer.
    pub fn start(&mut self) -> Result<Recording<'_, 'd, P>, Error> {
        // Disjoint field borrows: the transfer takes `rx` and `buffer`, the recording also needs
        // `scratch`.
        let transfer = self.rx.read_dma_circular(&mut self.buffer)?;
        Ok(Recording {
            transfer,
            scratch: &mut self.scratch[..],
            slot: self.slot,
            _part: PhantomData,
        })
    }

    /// The configured sample rate.
    pub fn sample_rate(&self) -> Rate {
        self.sample_rate
    }

    /// Which slot of each stereo frame is being kept.
    pub fn slot(&self) -> Slot {
        self.slot
    }
}

/// Builder returned by [`I2sMic::builder`].
///
/// The pin type parameters start as `()` and are replaced by [`MicBuilder::with_pins`]; `build`
/// is only implemented for real pin types, so forgetting the pins is a compile error.
pub struct MicBuilder<'d, P, I, C, B = (), W = (), D = ()> {
    i2s: I,
    channel: C,
    bclk: B,
    ws: W,
    din: D,
    sample_rate: Rate,
    slot: Slot,
    _part: PhantomData<P>,
    _lifetime: PhantomData<&'d ()>,
}

impl<'d, P: MicPart, I, C, B, W, D> MicBuilder<'d, P, I, C, B, W, D> {
    /// Supply the three I2S pins: bit clock, word select, and the microphone's data output.
    ///
    /// There is no MCLK — MEMS microphones of this family do not have one.
    pub fn with_pins<B2, W2, D2>(
        self,
        bclk: B2,
        ws: W2,
        din: D2,
    ) -> MicBuilder<'d, P, I, C, B2, W2, D2> {
        MicBuilder {
            i2s: self.i2s,
            channel: self.channel,
            bclk,
            ws,
            din,
            sample_rate: self.sample_rate,
            slot: self.slot,
            _part: PhantomData,
            _lifetime: PhantomData,
        }
    }

    /// Override [`DEFAULT_SAMPLE_RATE`].
    pub fn with_sample_rate(mut self, rate: Rate) -> Self {
        self.sample_rate = rate;
        self
    }

    /// Override which slot the microphone drives, for boards that tie the channel-select pin
    /// high rather than low.
    pub fn with_slot(mut self, slot: Slot) -> Self {
        self.slot = slot;
        self
    }
}

impl<'d, P, I, C, B, W, D> MicBuilder<'d, P, I, C, B, W, D>
where
    P: MicPart,
    I: Instance + 'd,
    C: DmaChannelFor<AnyI2s<'d>>,
    B: PeripheralOutput<'d>,
    W: PeripheralOutput<'d>,
    D: PeripheralInput<'d>,
{
    /// Configure the peripheral and take ownership of the buffers.
    ///
    /// `buffer` is the circular DMA buffer and sets how much slack there is between the hardware
    /// and your read loop; at 16 kHz each 8 KB is about 125 ms. It must stay under 65536 bytes,
    /// which is all the ESP32-S3's 16-bit `rx_eof_num` register can express.
    ///
    /// `scratch` is where [`Recording::read`] unloads the DMA. It bounds how far behind you may
    /// fall between reads: the hardware requires a pop to take *everything* buffered, so once
    /// more than `scratch.len()` bytes have accumulated the read reports
    /// [`Error::Overrun`]. Each raw byte is a quarter of a mono sample, so 4096 bytes
    /// ([`DEFAULT_SCRATCH_BYTES`]) covers 512 samples, or 32 ms at 16 kHz.
    pub fn build(
        self,
        descriptors: &'static mut [DmaDescriptor],
        buffer: &'static mut [u8],
        scratch: &'static mut [u8],
    ) -> Result<I2sMic<'d, P>, Error> {
        // Both slots are read and the unused one discarded on the way past; that is more
        // predictable than masking a slot off in hardware.
        let config = P::base_config()
            .with_sample_rate(self.sample_rate)
            .with_data_format(P::DATA_FORMAT)
            .with_channels(Channels::STEREO);

        let i2s = I2s::new(self.i2s, self.channel, config)?;
        let rx = i2s
            .i2s_rx
            .with_bclk(self.bclk)
            .with_ws(self.ws)
            .with_din(self.din)
            .build(descriptors);

        Ok(I2sMic {
            rx,
            buffer,
            scratch,
            slot: self.slot,
            sample_rate: self.sample_rate,
            _part: PhantomData,
        })
    }
}

/// An in-progress capture, produced by [`I2sMic::start`].
pub struct Recording<'a, 'd, P: MicPart> {
    transfer: DmaTransferRxCircular<'a, I2sRx<'d, Blocking>>,
    scratch: &'a mut [u8],
    slot: Slot,
    _part: PhantomData<P>,
}

impl<P: MicPart> Recording<'_, '_, P> {
    /// Read up to `out.len()` mono samples, returning how many were written.
    ///
    /// Never blocks: returns `Ok(0)` when the DMA has not produced a whole frame yet. Samples are
    /// raw, so apply [`crate::DcBlocker`] yourself if you want the microphone's DC offset gone.
    ///
    /// An `Err(Error::Overrun)` means the read loop fell behind and audio was lost; drop this
    /// value and call [`I2sMic::start`] again. To make that less likely, read more often, or
    /// enlarge the scratch and `out` — see [`MicBuilder::build`].
    pub fn read(&mut self, out: &mut [i16]) -> Result<usize, Error> {
        let take = self.take_size(out.len() * FRAME_BYTES)?;
        if take == 0 {
            return Ok(0);
        }
        let got = self.transfer.pop(&mut self.scratch[..take])?;
        Ok(unpack_mono(&self.scratch[..got], self.slot, P::SHIFT, out))
    }

    /// Discard `samples` worth of audio, for letting a microphone settle after power-up.
    ///
    /// MEMS parts need roughly a second before their DC offset stops dominating the signal. Note
    /// this throws the audio away without filtering it, so a [`crate::DcBlocker`] applied
    /// afterwards still starts from a cold state.
    pub fn discard(&mut self, samples: usize) -> Result<(), Error> {
        let mut left = samples;
        while left > 0 {
            let take = self.take_size(left * FRAME_BYTES)?;
            if take == 0 {
                continue;
            }
            let got = self.transfer.pop(&mut self.scratch[..take])?;
            left = left.saturating_sub(got / FRAME_BYTES);
        }
        Ok(())
    }

    /// How many raw bytes to pop: everything buffered, clamped to what we can absorb and rounded
    /// down to whole frames so the left/right phase never slips.
    ///
    /// A pop must take everything the hardware has buffered — asking for less is rejected — so
    /// clamping here is what converts "we fell behind" into an [`Error::Overrun`] on the pop.
    fn take_size(&mut self, wanted_bytes: usize) -> Result<usize, Error> {
        let available = self.transfer.available()?;
        if available < FRAME_BYTES {
            return Ok(0);
        }
        let capacity = self.scratch.len().min(wanted_bytes);
        Ok((available.min(capacity) / FRAME_BYTES) * FRAME_BYTES)
    }
}
