#![cfg_attr(not(test), no_std)]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! I2S audio output for [esp-hal], with a MAX98357A preset and a `no_std` WAV parser.
//!
//! ```text
//! let wav = Wav::parse(SOUND)?;
//!
//! let mut speaker = Max98357a::builder(peripherals.I2S0, peripherals.DMA_CH0)
//!     .with_pins(peripherals.GPIO15, peripherals.GPIO16, peripherals.GPIO7)
//!     .with_sample_rate(Rate::from_hz(wav.sample_rate()))
//!     .build(tx_descriptors, tx_buffer)?;
//!
//! let mut playback = speaker.start()?;
//! playback.write(&samples)?;
//! playback.flush()?;
//! ```
//!
//! See [`driver`] for the complete, compiled version of that example.
//!
//! # Wiring
//!
//! ```text
//! MAX98357A      ESP32-S3
//! ---------      --------
//! VIN    ------  5V         (3V3 works, quieter)
//! GND    ------  GND
//! BCLK   ------  bclk pin
//! LRC    ------  ws pin
//! DIN    ------  dout pin
//! GAIN   ------  float      9 dB
//! SD     ------  float/high enabled
//!         speaker on the +/- screw terminals
//! ```
//!
//! The amplifier is mono and its `SD` pin selects Left, Right or `(L+R)/2`. Every sample is
//! written to both slots, so playback is correct however `SD` is wired, and [`Wav`] downmixes
//! stereo files on the way through.
//!
//! # Using it from another project
//!
//! ```toml
//! [dependencies]
//! i2s-speaker = { git = "https://github.com/<you>/basicprograms", features = ["defmt"] }
//! esp-hal = { version = "~1.0", features = ["esp32s3", "unstable"] }
//! ```
//!
//! The library does not select a chip feature for `esp-hal`; your binary does, and cargo applies
//! it across the dependency graph. A consequence is that this crate cannot be built entirely on
//! its own — supply a chip when there is no binary to do it:
//!
//! ```text
//! cargo build -p i2s-speaker --features esp-hal/esp32s3
//! ```
//!
//! The WAV parser is tested on the host. Cargo reads `.cargo/config.toml` and
//! `rust-toolchain.toml` relative to the working directory, so run this from outside
//! `Rust/speaker` to escape the pinned xtensa target:
//!
//! ```text
//! cargo test --manifest-path Rust/speaker/crates/i2s-speaker/Cargo.toml --no-default-features
//! ```
//!
//! # Cargo features
//!
//! - `driver` (default) — the esp-hal driver. Without it only [`wav`] is compiled, which builds
//!   for the host so tests can run there.
//! - `defmt` — `defmt::Format` implementations for the public types.
//!
//! [esp-hal]: https://docs.espressif.com/projects/rust/esp-hal/latest/

pub mod wav;

#[cfg(feature = "driver")]
pub mod driver;
#[cfg(feature = "driver")]
pub mod part;

pub use wav::{Samples, Wav, WavError};

#[cfg(feature = "driver")]
pub use driver::{DEFAULT_SAMPLE_RATE, Error, I2sOut, Playback, SpeakerBuilder};

/// A MAX98357A amplifier.
///
/// This is [`I2sOut`] specialised to the part, so the entry point is `Max98357a::builder(..)`.
#[cfg(feature = "driver")]
pub type Max98357a<'d> = I2sOut<'d, part::Max98357a>;
