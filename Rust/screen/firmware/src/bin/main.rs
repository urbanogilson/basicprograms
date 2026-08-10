#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! Demo for a GME12864 (SSD1306) 128x64 OLED over I2C.
//!
//! Wiring:
//!
//! ```text
//! GME12864       ESP32-S3
//! --------       --------
//! VCC    ------  3V3
//! GND    ------  GND
//! SDA    ------  GPIO8
//! SCL    ------  GPIO9
//! ```
//!
//! Shows a splash, then a frame redrawn once a second with a border, two text sizes and a running
//! uptime counter. If the panel stays blank, run `cargo run --release --bin i2cscan` first - the
//! usual cause is the module being at 0x3D rather than 0x3C.

use core::fmt::Write;

use defmt::info;
use embedded_graphics::mono_font::MonoTextStyle;
use embedded_graphics::mono_font::ascii::FONT_9X18_BOLD;
use embedded_graphics::pixelcolor::BinaryColor;
use embedded_graphics::prelude::*;
use embedded_graphics::primitives::{PrimitiveStyle, Rectangle, RoundedRectangle};
use embedded_graphics::text::{Baseline, Text};
use esp_hal::clock::CpuClock;
use esp_hal::i2c::master::{Config, I2c};
use esp_hal::main;
use esp_hal::time::{Duration, Instant, Rate};
use oled_screen::{Screen, layout};
use panic_rtt_target as _;

// This creates a default app-descriptor required by the esp-idf bootloader.
// For more information see: <https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/system/app_image_format.html#application-description>
esp_bootloader_esp_idf::esp_app_desc!();

/// How long the splash stays up before the live frame takes over.
const SPLASH: Duration = Duration::from_millis(1500);
/// Frame interval. One second makes a stopped counter obvious at a glance.
const TICK: Duration = Duration::from_millis(1000);

/// A tiny `core::fmt` sink, so numbers can be formatted without an allocator.
struct Buf<const N: usize> {
    bytes: [u8; N],
    len: usize,
}

impl<const N: usize> Buf<N> {
    const fn new() -> Self {
        Self {
            bytes: [0; N],
            len: 0,
        }
    }

    fn clear(&mut self) {
        self.len = 0;
    }

    fn as_str(&self) -> &str {
        // Only ever written through `Write`, which supplies valid UTF-8, and truncation happens
        // on a character boundary below.
        core::str::from_utf8(&self.bytes[..self.len]).unwrap_or("")
    }
}

impl<const N: usize> Write for Buf<N> {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        for c in s.chars() {
            let mut encoded = [0u8; 4];
            let bytes = c.encode_utf8(&mut encoded).as_bytes();
            if self.len + bytes.len() > N {
                return Err(core::fmt::Error);
            }
            self.bytes[self.len..self.len + bytes.len()].copy_from_slice(bytes);
            self.len += bytes.len();
        }
        Ok(())
    }
}

#[allow(
    clippy::large_stack_frames,
    reason = "it's not unusual to allocate larger buffers etc. in main"
)]
#[main]
fn main() -> ! {
    rtt_target::rtt_init_defmt!();

    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    // 400 kHz rather than the 100 kHz default: a full frame is 1 KB, so ~25 ms here versus
    // ~100 ms, which is the difference between a usable refresh and a visible crawl.
    let i2c = I2c::new(
        peripherals.I2C0,
        Config::default().with_frequency(Rate::from_khz(400)),
    )
    .expect("invalid I2C config")
    .with_sda(peripherals.GPIO8)
    .with_scl(peripherals.GPIO9);

    // If this fails the panel is almost certainly at another address - run the i2cscan binary.
    let mut screen = Screen::new(i2c).expect("no display found - try `cargo run --bin i2cscan`");
    info!("display initialised at 0x3c");

    screen.clear();
    screen.centered(1, "GME12864").expect("draw failed");
    screen.centered(3, "SSD1306 128x64").expect("draw failed");
    screen.flush().expect("flush failed");

    let boot = Instant::now();
    let splash_until = Instant::now() + SPLASH;
    while Instant::now() < splash_until {}

    let big = MonoTextStyle::new(&FONT_9X18_BOLD, BinaryColor::On);
    let outline = PrimitiveStyle::with_stroke(BinaryColor::On, 1);
    let mut text: Buf<24> = Buf::new();
    let mut frames = 0u32;

    loop {
        let started = Instant::now();
        let secs = boot.elapsed().as_millis() / 1000;
        frames += 1;

        screen.clear();

        // Border, inset by one pixel so the stroke is fully on-panel.
        screen
            .draw(|d| {
                RoundedRectangle::with_equal_corners(
                    Rectangle::new(
                        Point::new(0, 0),
                        Size::new(layout::WIDTH, layout::HEIGHT),
                    ),
                    Size::new(4, 4),
                )
                .into_styled(outline)
                .draw(d)?;
                Ok(())
            })
            .expect("draw failed");

        screen.centered(0, "ESP32-S3 OLED").expect("draw failed");

        // Uptime in the larger font, centred by hand since it uses a different font width.
        text.clear();
        let _ = write!(text, "{:02}:{:02}", secs / 60, secs % 60);
        let width = text.as_str().len() as i32 * 9;
        screen
            .draw(|d| {
                Text::with_baseline(
                    text.as_str(),
                    Point::new((layout::WIDTH as i32 - width) / 2, 22),
                    big,
                    Baseline::Top,
                )
                .draw(d)?;
                Ok(())
            })
            .expect("draw failed");

        text.clear();
        let _ = write!(text, "frames {frames}");
        screen.centered(5, text.as_str()).expect("draw failed");

        screen.flush().expect("flush failed");

        if frames == 1 {
            info!("first frame drawn in {} ms", started.elapsed().as_millis());
        }

        let next = started + TICK;
        while Instant::now() < next {}
    }
}
