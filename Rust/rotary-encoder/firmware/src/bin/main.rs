#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! Reads a V635 rotary encoder and logs detents and button events.
//!
//! Wiring:
//!
//! ```text
//! V635           ESP32-S3
//! ----           --------
//! GND    ------  GND
//! 5V     ------  3V3        <- NOT 5V: ESP32 GPIOs are not 5V tolerant and this
//!                              module's pull-ups would drag the signals to 5V
//! S1     ------  GPIO10     quadrature A
//! S2     ------  GPIO11     quadrature B
//! KEY    ------  GPIO12     push button, active low
//! ```
//!
//! Turn the knob to move the position; click to reset it. Every log line carries the raw
//! quadrature transition count, which is what lets you check the detent ratio on your own unit:
//! turn exactly ten clicks and read `transitions`. Ten detents should be 40 transitions with the
//! default [`Encoder::DEFAULT_TRANSITIONS_PER_DETENT`]. If it is 10, build the encoder with
//! `with_transitions_per_detent(1)`; if 20, use 2.

use defmt::info;
use esp_hal::clock::CpuClock;
use esp_hal::gpio::{Input, InputConfig, Pull};
use esp_hal::main;
use esp_hal::time::{Duration, Instant};
use panic_rtt_target as _;
use rotary_encoder::{Button, ButtonEvent, Direction, Encoder};

// This creates a default app-descriptor required by the esp-idf bootloader.
// For more information see: <https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/system/app_image_format.html#application-description>
esp_bootloader_esp_idf::esp_app_desc!();

/// Poll interval. A knob turned by hand cannot outrun 1 ms sampling, and polling in order is what
/// the quadrature state table needs - interrupts would fire hundreds of times per detent on a
/// bouncing contact and buy nothing.
const POLL: Duration = Duration::from_micros(1000);

#[allow(
    clippy::large_stack_frames,
    reason = "it's not unusual to allocate larger buffers etc. in main"
)]
#[main]
fn main() -> ! {
    rtt_target::rtt_init_defmt!();

    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    // Internal pull-ups: required for a bare encoder, harmless in parallel with a module's own.
    // All three signals are active low, so a closed contact reads as low.
    let pull_up = InputConfig::default().with_pull(Pull::Up);
    let s1 = Input::new(peripherals.GPIO10, pull_up);
    let s2 = Input::new(peripherals.GPIO11, pull_up);
    let key = Input::new(peripherals.GPIO12, pull_up);

    let mut encoder = Encoder::new();
    let mut button = Button::new();
    let mut position: i32 = 0;

    info!(
        "rotary encoder ready: S1=GPIO10 S2=GPIO11 KEY=GPIO12, {} transitions/detent",
        Encoder::DEFAULT_TRANSITIONS_PER_DETENT
    );

    let boot = Instant::now();
    loop {
        let now_ms = boot.elapsed().as_millis() as u32;

        match encoder.update(s1.is_low(), s2.is_low()) {
            Some(Direction::Clockwise) => {
                position += 1;
                info!("CW   position={} transitions={}", position, encoder.transitions());
            }
            Some(Direction::CounterClockwise) => {
                position -= 1;
                info!("CCW  position={} transitions={}", position, encoder.transitions());
            }
            None => {}
        }

        match button.update(key.is_low(), now_ms) {
            Some(ButtonEvent::Pressed) => {
                position = 0;
                encoder.reset();
                info!("press -> position reset");
            }
            Some(ButtonEvent::Released) => info!("release"),
            Some(ButtonEvent::LongPress) => info!("long press"),
            None => {}
        }

        let next = Instant::now() + POLL;
        while Instant::now() < next {}
    }
}
