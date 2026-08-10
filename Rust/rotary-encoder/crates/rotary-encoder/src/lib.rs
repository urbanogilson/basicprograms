// `not(test)` so the host test build gets std, which the tests use for Vec.
#![cfg_attr(not(test), no_std)]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! Bounce-tolerant decoding for incremental rotary encoders such as the V635 / KY-040 / EC11.
//!
//! ```
//! # use rotary_encoder::{Button, ButtonEvent, Direction, Encoder};
//! let mut encoder = Encoder::new();
//! let mut button = Button::new();
//! let mut position = 0i32;
//!
//! # let samples = [(false, false, false, 0u32), (false, true, false, 1)];
//! # for (s1, s2, key, now_ms) in samples {
//! // ...sampled from your pins, active-low so `is_low()` means true here
//! match encoder.update(s1, s2) {
//!     Some(Direction::Clockwise) => position += 1,
//!     Some(Direction::CounterClockwise) => position -= 1,
//!     None => {}
//! }
//! if button.update(key, now_ms) == Some(ButtonEvent::Pressed) {
//!     position = 0;
//! }
//! # }
//! ```
//!
//! # Why software, not the PCNT peripheral
//!
//! The ESP32-S3 has a hardware pulse counter with a glitch filter, which looks like the obvious
//! tool. It is not: the filter tops out at 1023 APB_CLK cycles, i.e. 12.8 µs at 80 MHz, while
//! mechanical contact bounce lasts 1–5 ms — some 100–400× longer. That filter exists for
//! electrical noise on optical or Hall sensors, not for switch contacts.
//!
//! [`Encoder`] handles bounce structurally instead of by timing. It tracks the full quadrature
//! state and only reports a step once a complete detent has been travelled, so chatter cancels
//! itself out and a physically impossible transition is discarded rather than guessed at.
//!
//! # Wiring
//!
//! ```text
//! V635           MCU
//! ----           ---
//! GND    ------  GND
//! 5V     ------  3V3      <- NOT 5V on an ESP32; see below
//! S1     ------  input with pull-up
//! S2     ------  input with pull-up
//! KEY    ------  input with pull-up
//! ```
//!
//! **Do not power these modules from 5 V on an ESP32.** ESP32 GPIOs are 3.3 V and not 5 V
//! tolerant, and the module's on-board pull-ups would drag S1/S2/KEY to 5 V, damaging the pins.
//! The module works fine at 3.3 V. Enable internal pull-ups regardless: they are required for a
//! bare encoder and harmless alongside a module's own.
//!
//! # Portability
//!
//! This crate takes sampled levels as plain `bool`s and the time as a `u32`, so it depends on no
//! HAL, works on any platform, and — usefully — compiles for the host, which is how its state
//! machines are unit-tested.
//!
//! # Cargo features
//!
//! - `defmt` — `defmt::Format` implementations for [`Direction`] and [`ButtonEvent`].

pub mod button;
pub mod encoder;

pub use button::{Button, ButtonEvent};
pub use encoder::{Direction, Encoder};
