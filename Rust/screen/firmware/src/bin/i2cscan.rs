#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

//! Diagnostic: reports every device that responds on the I2C bus.
//!
//! ```sh
//! cargo run --release --bin i2cscan
//! ```
//!
//! A display at the wrong address is indistinguishable from a broken one: writes to an absent I2C
//! device are simply unacknowledged, so the panel stays blank and nothing reports an error. This
//! separates the three cases in one run:
//!
//! - **0x3C or 0x3D found** — the panel is alive and addressable. If it is 0x3D, construct the
//!   screen with `Screen::with_address(i2c, oled_screen::ALTERNATE_ADDRESS)`.
//! - **Nothing found** — wiring or power. Check SDA/SCL are not swapped, that VCC is 3V3, and
//!   that both grounds are common.
//! - **Something else found** — you are talking to a different device than you think.

use defmt::{info, warn};
use esp_hal::clock::CpuClock;
use esp_hal::i2c::master::{Config, I2c};
use esp_hal::main;
use esp_hal::time::{Duration, Instant, Rate};
use panic_rtt_target as _;

esp_bootloader_esp_idf::esp_app_desc!();

/// Reserved addresses sit outside 0x08..=0x77, so scanning them tells you nothing.
const FIRST: u8 = 0x08;
const LAST: u8 = 0x77;

#[main]
fn main() -> ! {
    rtt_target::rtt_init_defmt!();

    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    // Scan at the 100 kHz default: slower is more forgiving of marginal wiring, and this is
    // exactly the situation where the wiring is in question.
    let mut i2c = I2c::new(
        peripherals.I2C0,
        Config::default().with_frequency(Rate::from_khz(100)),
    )
    .expect("invalid I2C config")
    .with_sda(peripherals.GPIO8)
    .with_scl(peripherals.GPIO9);

    info!("scanning I2C bus (SDA=GPIO8, SCL=GPIO9)...");

    let mut found = 0u32;
    for address in FIRST..=LAST {
        // A zero-length write addresses the device and stops: enough to see whether anything
        // acknowledges, without altering the state of whatever is out there.
        if i2c.write(address, &[]).is_ok() {
            found += 1;
            match address {
                0x3C | 0x3D => info!("  0x{:02x}  <- SSD1306 display", address),
                _ => info!("  0x{:02x}", address),
            }
        }
    }

    if found == 0 {
        warn!("no devices responded - check wiring, power, and that SDA/SCL are not swapped");
    } else {
        info!("scan complete: {} device(s)", found);
    }

    // Nothing left to do; idle rather than spinning flat out.
    loop {
        let wait = Instant::now() + Duration::from_millis(1000);
        while Instant::now() < wait {}
    }
}
