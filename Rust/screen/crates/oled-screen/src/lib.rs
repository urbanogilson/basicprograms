#![no_std]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! An ergonomic wrapper around [`ssd1306`] + [`embedded_graphics`] for 128x64 OLED modules such
//! as the GME12864.
//!
//! ```no_run
//! # fn demo<I: embedded_hal::i2c::I2c>(i2c: I) -> Result<(), oled_screen::Error> {
//! use oled_screen::Screen;
//!
//! let mut screen = Screen::new(i2c)?;   // 0x3C; use `with_address` for 0x3D
//! screen.clear();
//! screen.centered(0, "Hello")?;
//! screen.line(2, "second row")?;
//! screen.flush()?;                      // nothing is visible until this runs
//! # Ok(())
//! # }
//! ```
//!
//! # Why this is a wrapper, not a driver
//!
//! Unlike a bespoke peripheral, the SSD1306 is thoroughly served by the existing `ssd1306` crate,
//! so re-implementing its command set and a font renderer would re-solve a solved problem. What
//! is worth owning is the ergonomics: initialising, blanking a panel that powers up full of
//! noise, and placing text by row rather than by pixel.
//!
//! Because the wrapper is generic over [`embedded_hal::i2c::I2c`] it depends on no HAL at all, so
//! it works on any chip whose HAL implements that trait - and builds for the host, which is what
//! lets [`layout`] be unit-tested.
//!
//! # Wiring
//!
//! ```text
//! GME12864       ESP32-S3
//! --------       --------
//! VCC    ------  3V3
//! GND    ------  GND
//! SDA    ------  I2C data pin
//! SCL    ------  I2C clock pin
//! ```
//!
//! The module has its own pull-ups, so no external resistors are needed. Drive the bus at 400 kHz
//! if you can: a full frame is 1 KB, which is ~25 ms at 400 kHz but ~100 ms at the 100 kHz
//! default.
//!
//! An unresponsive panel is nearly always the address rather than a bug - 0x3C and 0x3D are both
//! common, and writes to an absent device are simply unacknowledged rather than failing loudly.
//! The firmware's `i2cscan` binary reports what is actually on the bus.
//!
//! # Cargo features
//!
//! - `defmt` — a `defmt::Format` implementation for [`Error`].

pub mod layout;
pub mod screen;

pub use screen::{ALTERNATE_ADDRESS, DEFAULT_ADDRESS, Display, Error, Screen};

/// Re-exported so callers can set brightness without depending on `ssd1306` directly.
pub use ssd1306::prelude::Brightness;
