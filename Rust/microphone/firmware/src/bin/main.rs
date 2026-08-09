#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! Streams INMP441 audio over USB CDC.
//!
//! Wiring:
//!
//! ```text
//! INMP441        ESP32-S3
//! -------        --------
//! VDD    ------  3V3        (not 5V)
//! GND    ------  GND
//! SCK    ------  GPIO4      bit clock
//! WS     ------  GPIO5      word select
//! SD     ------  GPIO6      mic data out -> esp32 in
//! L/R    ------  GND        mic drives the left slot; must be tied, never floating
//! ```
//!
//! After a short warm-up the firmware streams 16 kHz mono PCM continuously out the built-in USB
//! serial port as framed binary. Point `tools/scope.py` at it for a live waveform/spectrum, or
//! `tools/record.py` to write a wav of any length.
//!
//! The microphone itself lives in the `i2s-mic` crate; what is left here is the wire protocol and
//! the USB transport.

use defmt::{info, warn};
use esp_hal::Blocking;
use esp_hal::clock::CpuClock;
use esp_hal::dma_buffers;
use esp_hal::main;
use esp_hal::time::{Duration, Instant, Rate};
use esp_hal::usb_serial_jtag::{UsbSerialJtag, UsbSerialJtagTx};
use i2s_mic::{DcBlocker, Inmp441, Level};
use panic_rtt_target as _;

// This creates a default app-descriptor required by the esp-idf bootloader.
// For more information see: <https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/system/app_image_format.html#application-description>
esp_bootloader_esp_idf::esp_app_desc!();

const SAMPLE_RATE: u32 = 16_000;

/// Discard this much audio before streaming. The INMP441's high-pass filter takes roughly a
/// second to settle after power-up, and the DC blocker needs to converge; without this the
/// stream opens with a loud decaying thump.
const WARMUP_SAMPLES: usize = SAMPLE_RATE as usize;

/// Samples per level-meter report, ~500 ms. The scope shows levels far better than text does,
/// so this only needs to be frequent enough to prove the firmware is alive.
const METER_SAMPLES: usize = SAMPLE_RATE as usize / 2;

/// Samples pulled from the microphone per read. Also sizes the driver's scratch buffer,
/// at 8 raw bytes per mono sample, giving 32 ms of slack between reads.
const READ_SAMPLES: usize = 512;

/// Wire format, little-endian throughout:
///
/// ```text
/// offset  size  field
/// 0       4     magic "MIC1"
/// 4       2     seq, wraps - the host uses gaps to count dropped frames
/// 6       2     payload length in bytes (always PAYLOAD_BYTES here)
/// 8       512   PCM samples, i16 mono
/// ```
const MAGIC: [u8; 4] = *b"MIC1";
const HEADER_BYTES: usize = 8;
/// 256 samples = 16 ms per frame, 62.5 frames/s, ~32.5 KB/s on the wire.
const FRAME_SAMPLES: usize = 256;
const PAYLOAD_BYTES: usize = FRAME_SAMPLES * 2;
const FRAME_BYTES: usize = HEADER_BYTES + PAYLOAD_BYTES;

/// How long to keep retrying a frame before giving up on it. Generous compared to the 16 ms a
/// frame represents, but short enough that a vanished host doesn't stall audio for long.
const FRAME_TIMEOUT: Duration = Duration::from_millis(50);

/// Timeout used once the host has already missed a frame. Spending 50 ms per frame while nobody
/// is listening starves the I2S DMA and produces a steady stream of overruns; this keeps the
/// idle path nearly free while still noticing the moment a reader shows up.
const PROBE_TIMEOUT: Duration = Duration::from_millis(1);

#[allow(
    clippy::large_stack_frames,
    reason = "it's not unusual to allocate larger buffers etc. in main"
)]
#[main]
fn main() -> ! {
    rtt_target::rtt_init_defmt!();

    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    // The DMA buffer is ~250 ms of slack, well under the 65536-byte limit the S3 imposes via its
    // 16-bit rx_eof_num register. The macro's second buffer becomes the driver's scratch space:
    // a pop has to take everything the DMA has buffered, so it bounds how far a read may fall
    // behind before reporting an overrun.
    let (rx_buffer, rx_descriptors, scratch, _) = dma_buffers!(8 * 4092, READ_SAMPLES * 8);

    let mut mic = Inmp441::builder(peripherals.I2S0, peripherals.DMA_CH0)
        .with_pins(peripherals.GPIO4, peripherals.GPIO5, peripherals.GPIO6)
        .with_sample_rate(Rate::from_hz(SAMPLE_RATE))
        .build(rx_descriptors, rx_buffer, scratch)
        .expect("invalid microphone config");

    // `split` returns (rx, tx) - the reverse of the usual convention.
    let (_usb_rx, mut usb_tx) = UsbSerialJtag::new(peripherals.USB_DEVICE).split();

    // Buffers live in main because `.clippy.toml` sets stack-size-threshold = 1024
    // and the crate denies `large_stack_frames` everywhere else.
    let mut samples = [0i16; READ_SAMPLES];
    let mut frame = [0u8; FRAME_BYTES];
    frame[..4].copy_from_slice(&MAGIC);
    frame[6..8].copy_from_slice(&(PAYLOAD_BYTES as u16).to_le_bytes());

    info!("I2S started: {} Hz mono, warming up...", SAMPLE_RATE);

    let mut dc = DcBlocker::new();
    let mut level = Level::new();
    let mut warmed = 0usize;
    let mut seq: u16 = 0;
    let mut filled = 0usize;
    let mut dropped: u32 = 0;
    let mut overruns: u32 = 0;
    // True once a frame has been abandoned, i.e. nothing appears to be reading the port.
    let mut stalled = false;

    loop {
        let mut recording = mic.start().expect("failed to start capture");

        loop {
            // Falling behind is not worth a panic - drop the recording and resync.
            let n = match recording.read(&mut samples) {
                Ok(n) => n,
                Err(e) => {
                    warn!("microphone overrun ({}), resyncing", e);
                    overruns += 1;
                    filled = 0;
                    break;
                }
            };
            let block = &mut samples[..n];
            dc.process(block);

            // Warm-up runs through the DC blocker rather than being discarded, so the filter has
            // converged by the time the first frame goes out.
            if warmed < WARMUP_SAMPLES {
                warmed += n;
                if warmed >= WARMUP_SAMPLES {
                    info!("streaming to USB serial");
                }
                continue;
            }

            level.extend(block);
            if level.len() >= METER_SAMPLES {
                let stats = level.take();
                info!(
                    "rms={} peak={} dBFS={} dropped={} overruns={}",
                    stats.rms, stats.peak, stats.dbfs, dropped, overruns
                );
            }

            for &s in block.iter() {
                let at = HEADER_BYTES + filled * 2;
                frame[at..at + 2].copy_from_slice(&s.to_le_bytes());
                filled += 1;
                if filled < FRAME_SAMPLES {
                    continue;
                }

                frame[4..6].copy_from_slice(&seq.to_le_bytes());
                // seq advances even on a drop, so the host sees the gap rather than a stall.
                seq = seq.wrapping_add(1);
                filled = 0;

                let timeout = if stalled { PROBE_TIMEOUT } else { FRAME_TIMEOUT };
                stalled = !write_frame(&mut usb_tx, &frame, timeout);
                if stalled {
                    dropped += 1;
                }
            }
        }
    }
}

/// Push one frame out the USB serial port, returning false if it had to be abandoned.
///
/// `UsbSerialJtag::write` busy-spins forever when nothing is draining the port, and its error
/// type is `Infallible` so it can never tell you. Retrying `write_byte_nb` against a deadline
/// keeps the firmware running whether or not a host is listening; a partial frame is fine
/// because the host resynchronises on the magic.
fn write_frame(tx: &mut UsbSerialJtagTx<'_, Blocking>, bytes: &[u8], timeout: Duration) -> bool {
    let deadline = Instant::now() + timeout;
    for &b in bytes {
        loop {
            match tx.write_byte_nb(b) {
                Ok(()) => break,
                Err(nb::Error::WouldBlock) => {
                    if Instant::now() >= deadline {
                        return false;
                    }
                }
                Err(nb::Error::Other(_)) => return false,
            }
        }
    }
    true
}
