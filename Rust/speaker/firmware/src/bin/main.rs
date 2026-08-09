#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! Plays an embedded WAV clip through a MAX98357A I2S amplifier.
//!
//! Wiring:
//!
//! ```text
//! MAX98357A      ESP32-S3
//! ---------      --------
//! VIN    ------  5V         (3V3 works, quieter)
//! GND    ------  GND
//! BCLK   ------  GPIO15     bit clock
//! LRC    ------  GPIO16     word select
//! DIN    ------  GPIO7      data out
//! GAIN   ------  float      9 dB
//! SD     ------  float/high enabled
//!         speaker on the +/- screw terminals
//! ```
//!
//! The clip repeats with a short gap so it can be heard without resetting the board. Swap in your
//! own audio with `tools/prepare_wav.sh some-file.mp3`.

use defmt::{info, warn};
use esp_hal::clock::CpuClock;
use esp_hal::dma_buffers;
use esp_hal::main;
use esp_hal::time::{Duration, Instant, Rate};
use i2s_speaker::{Max98357a, Wav};
use panic_rtt_target as _;

// This creates a default app-descriptor required by the esp-idf bootloader.
// For more information see: <https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/system/app_image_format.html#application-description>
esp_bootloader_esp_idf::esp_app_desc!();

/// Regenerate with `tools/prepare_wav.sh <input>`. Embedded in flash, so its size is firmware
/// size: 2 bytes per sample, i.e. 64 KB per second at 32 kHz. The board's `factory` partition is
/// 15.6 MB, so there is room for minutes of audio.
static SOUND: &[u8] = include_bytes!("../../assets/sound.wav");

/// Silence between repeats.
const GAP: Duration = Duration::from_millis(1500);

/// Samples staged per write. The DMA ring, not this, sets how much slack playback has.
const CHUNK_SAMPLES: usize = 256;

#[allow(
    clippy::large_stack_frames,
    reason = "it's not unusual to allocate larger buffers etc. in main"
)]
#[main]
fn main() -> ! {
    rtt_target::rtt_init_defmt!();

    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    let wav = match Wav::parse(SOUND) {
        Ok(wav) => wav,
        Err(e) => panic!("embedded clip is not playable: {:?}", e),
    };
    info!(
        "clip: {} Hz, {} ch, {} frames, {} ms",
        wav.sample_rate(),
        wav.channels(),
        wav.frames(),
        wav.duration_ms()
    );

    // ~128 ms of audio in flight, so a slow loop iteration cannot starve the amplifier.
    let (_, _, tx_buffer, tx_descriptors) = dma_buffers!(0, 8 * 4092);

    // The clip's own rate drives the peripheral, so files at other rates play at the right pitch.
    let mut speaker = Max98357a::builder(peripherals.I2S0, peripherals.DMA_CH0)
        .with_pins(peripherals.GPIO15, peripherals.GPIO16, peripherals.GPIO7)
        .with_sample_rate(Rate::from_hz(wav.sample_rate()))
        .build(tx_descriptors, tx_buffer)
        .expect("invalid speaker config");

    let mut chunk = [0i16; CHUNK_SAMPLES];

    loop {
        let started = Instant::now();
        let mut played = 0usize;
        let mut underruns = 0u32;
        let mut samples = wav.samples();

        let mut playback = speaker.start().expect("failed to start playback");
        loop {
            // Refill the chunk from the clip; stop once the file is exhausted.
            let mut staged = 0;
            for slot in chunk.iter_mut() {
                match samples.next() {
                    Some(s) => {
                        *slot = s;
                        staged += 1;
                    }
                    None => break,
                }
            }
            if staged == 0 {
                break;
            }

            // `write` takes what fits and returns how much; spin until the whole chunk is queued.
            let mut queued = 0;
            while queued < staged {
                queued += playback
                    .write(&chunk[queued..staged])
                    .expect("failed to queue audio");
                if playback.underrun().unwrap_or(false) {
                    underruns += 1;
                }
            }
            played += staged;
        }

        // Circular DMA replays the ring, so flush silence before stopping or the tail repeats.
        playback.flush().expect("failed to flush playback");

        if underruns > 0 {
            warn!("{} underruns - the DMA ran dry during playback", underruns);
        }
        info!(
            "played {} samples in {} ms",
            played,
            started.elapsed().as_millis()
        );

        let pause = Instant::now();
        while pause.elapsed() < GAP {}
    }
}
