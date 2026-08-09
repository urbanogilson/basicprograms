#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! Diagnostic: plays a continuous 1 kHz tone, with no WAV file involved.
//!
//! ```sh
//! cargo run --release --bin tone
//! ```
//!
//! This narrows down a speaker that only makes noise, by removing three variables at once — the
//! WAV parser, the file data, and the start/flush/stop cycle in `main`. It just opens the I2S once
//! and feeds a generated sine forever.
//!
//! - **Clean steady tone** — the I2S path and wiring are fine, so the fault is in how `main` feeds
//!   the clip. Check the sample rate reported at startup against the file.
//! - **Still noise** — the fault is the I2S format or the wiring. Slot width is the usual
//!   culprit: a MAX98357A produces noise on 16-bit slots even though its datasheet permits them,
//!   which is why `SLOT_FORMAT` below defaults to 32-bit. Also confirm `BCLK` and `LRC` are not
//!   swapped, which sounds identical to a format mismatch.
//! - **Silence** — the amplifier is shut down or unpowered: check `SD` is floating or high, and
//!   that `VIN` has 5 V.

use defmt::info;
use esp_hal::clock::CpuClock;
use esp_hal::dma_buffers;
use esp_hal::i2s::master::DataFormat;
use esp_hal::main;
use esp_hal::time::Rate;
use i2s_speaker::Max98357a;
use panic_rtt_target as _;

esp_bootloader_esp_idf::esp_app_desc!();

/// 32-bit slots give a 64x BCLK/LRCLK ratio; `Data16Channel16` gives 32x.
const SLOT_FORMAT: DataFormat = DataFormat::Data32Channel32;

const SAMPLE_RATE: u32 = 16_000;

/// Exactly 16 samples per period at 16 kHz, so the table alone produces a true 1 kHz tone with
/// no phase drift: `round(12000 * sin(2*pi*n/16))`.
const SINE_1KHZ: [i16; 16] = [
    0, 4592, 8485, 11087, 12000, 11087, 8485, 4592, 0, -4592, -8485, -11087, -12000, -11087, -8485,
    -4592,
];

#[allow(
    clippy::large_stack_frames,
    reason = "it's not unusual to allocate larger buffers etc. in main"
)]
#[main]
fn main() -> ! {
    rtt_target::rtt_init_defmt!();

    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    let (_, _, tx_buffer, tx_descriptors) = dma_buffers!(0, 8 * 4092);

    let mut speaker = Max98357a::builder(peripherals.I2S0, peripherals.DMA_CH0)
        .with_pins(peripherals.GPIO15, peripherals.GPIO16, peripherals.GPIO7)
        .with_sample_rate(Rate::from_hz(SAMPLE_RATE))
        .with_data_format(SLOT_FORMAT)
        .build(tx_descriptors, tx_buffer)
        .expect("invalid speaker config");

    info!(
        "1 kHz tone: {} Hz, {} bit data in {} bit slots (BCLK/LRCLK = {}x)",
        SAMPLE_RATE,
        SLOT_FORMAT.data_bits(),
        SLOT_FORMAT.channel_bits(),
        SLOT_FORMAT.channel_bits() as u32 * 2
    );

    // One long transfer, never stopped: nothing here but the I2S path itself.
    let mut playback = speaker.start().expect("failed to start playback");
    let mut phase = 0usize;
    let mut chunk = [0i16; 256];

    loop {
        for slot in chunk.iter_mut() {
            *slot = SINE_1KHZ[phase % SINE_1KHZ.len()];
            phase = phase.wrapping_add(1);
        }

        let mut queued = 0;
        while queued < chunk.len() {
            queued += playback
                .write(&chunk[queued..])
                .expect("failed to queue audio");
        }
    }
}
