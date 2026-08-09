#![cfg_attr(not(test), no_std)]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! I2S MEMS microphone driver for [esp-hal], with an INMP441 preset.
//!
//! ```text
//! let mut mic = Inmp441::builder(peripherals.I2S0, peripherals.DMA_CH0)
//!     .with_pins(peripherals.GPIO4, peripherals.GPIO5, peripherals.GPIO6)
//!     .with_sample_rate(Rate::from_hz(16_000))
//!     .build(rx_descriptors, rx_buffer, scratch)?;
//!
//! let mut recording = mic.start()?;
//! let n = recording.read(&mut samples)?;
//! ```
//!
//! See [`driver`] for the complete, compiled version of that example.
//!
//! # Wiring
//!
//! ```text
//! INMP441        ESP32-S3
//! -------        --------
//! VDD    ------  3V3        (not 5V)
//! GND    ------  GND
//! SCK    ------  bclk pin
//! WS     ------  ws pin
//! SD     ------  din pin
//! L/R    ------  GND        Slot::Left; tie to VDD for Slot::Right, never leave floating
//! ```
//!
//! # Using it from another project
//!
//! ```toml
//! [dependencies]
//! i2s-mic = { git = "https://github.com/<you>/basicprograms", features = ["defmt"] }
//! esp-hal = { version = "~1.0", features = ["esp32s3", "unstable"] }
//! ```
//!
//! The library does not select a chip feature for `esp-hal`; your binary does, and cargo applies
//! it across the dependency graph. A consequence is that this crate cannot be built entirely on
//! its own — supply a chip when there is no binary to do it:
//!
//! ```text
//! cargo build -p i2s-mic --features esp-hal/esp32s3
//! ```
//!
//! The portable half is tested on the host. Cargo reads `.cargo/config.toml` and
//! `rust-toolchain.toml` relative to the working directory, so run this from outside
//! `Rust/microphone` to escape the pinned xtensa target:
//!
//! ```text
//! cargo test --manifest-path Rust/microphone/crates/i2s-mic/Cargo.toml --no-default-features
//! ```
//!
//! # Cargo features
//!
//! - `driver` (default) — the esp-hal driver. Without it only the portable half ([`dsp`],
//!   [`level`], [`format`]) is compiled, which builds for the host so tests can run there.
//! - `defmt` — `defmt::Format` implementations for the public types.
//!
//! [esp-hal]: https://docs.espressif.com/projects/rust/esp-hal/latest/

pub mod dsp;
pub mod format;
pub mod level;

#[cfg(feature = "driver")]
pub mod driver;
#[cfg(feature = "driver")]
pub mod part;

pub use dsp::DcBlocker;
pub use format::{Slot, unpack_mono};
pub use level::{Level, LevelStats, dbfs};

#[cfg(feature = "driver")]
pub use driver::{
    DEFAULT_SAMPLE_RATE, DEFAULT_SCRATCH_BYTES, Error, I2sMic, MicBuilder, Recording,
};

/// An INMP441 microphone.
///
/// This is [`I2sMic`] specialised to the part, so the entry point is `Inmp441::builder(..)`.
#[cfg(feature = "driver")]
pub type Inmp441<'d> = I2sMic<'d, part::Inmp441>;
