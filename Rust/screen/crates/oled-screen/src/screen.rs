//! The [`Screen`] wrapper.

use display_interface::DisplayError;
use embedded_graphics::mono_font::MonoTextStyle;
use embedded_graphics::mono_font::ascii::FONT_6X10;
use embedded_graphics::pixelcolor::BinaryColor;
use embedded_graphics::prelude::*;
use embedded_graphics::text::{Baseline, Text};
use embedded_hal::i2c::I2c;
use ssd1306::mode::BufferedGraphicsMode;
use ssd1306::prelude::*;
use ssd1306::{I2CDisplayInterface, Ssd1306};

use crate::layout;

/// The default I2C address of an SSD1306 module.
pub const DEFAULT_ADDRESS: u8 = 0x3C;
/// The address used by modules with their select resistor moved.
pub const ALTERNATE_ADDRESS: u8 = 0x3D;

/// The concrete display type this wrapper drives: a 128x64 panel with a RAM framebuffer.
pub type Display<I2C> =
    Ssd1306<I2CInterface<I2C>, DisplaySize128x64, BufferedGraphicsMode<DisplaySize128x64>>;

/// Something went wrong talking to the panel.
///
/// In practice this is almost always a wiring or address problem rather than a bug: writes to an
/// absent I2C device simply go unacknowledged. Run the `i2cscan` binary to find out which.
// `DisplayError` is #[non_exhaustive] and derives only Debug, so this cannot be Copy or Eq.
#[derive(Debug)]
pub struct Error(pub DisplayError);

impl From<DisplayError> for Error {
    fn from(e: DisplayError) -> Self {
        Self(e)
    }
}

#[cfg(feature = "defmt")]
impl defmt::Format for Error {
    fn format(&self, f: defmt::Formatter) {
        // DisplayError is #[non_exhaustive] and not defmt-aware, so map the variants we care
        // about and fall back for the rest.
        let s = match self.0 {
            DisplayError::InvalidFormatError => "invalid format",
            DisplayError::BusWriteError => "bus write failed (check wiring and address)",
            DisplayError::DCError => "data/command error",
            DisplayError::CSError => "chip-select error",
            DisplayError::DataFormatNotImplemented => "data format not implemented",
            DisplayError::RSError => "reset error",
            DisplayError::OutOfBoundsError => "out of bounds",
            _ => "unknown display error",
        };
        defmt::write!(f, "{}", s)
    }
}

/// A 128x64 SSD1306 OLED on an I2C bus.
///
/// Nothing reaches the panel until [`flush`](Self::flush) is called: drawing only touches a RAM
/// framebuffer.
///
/// ```no_run
/// # fn demo<I: embedded_hal::i2c::I2c>(i2c: I) -> Result<(), oled_screen::Error> {
/// use oled_screen::Screen;
///
/// let mut screen = Screen::new(i2c)?;
/// screen.clear();
/// screen.centered(0, "Hello")?;
/// screen.line(2, "second row")?;
/// screen.flush()?;
/// # Ok(())
/// # }
/// ```
pub struct Screen<I2C> {
    display: Display<I2C>,
    style: MonoTextStyle<'static, BinaryColor>,
}

impl<I2C: I2c> Screen<I2C> {
    /// Initialise a panel at [`DEFAULT_ADDRESS`].
    pub fn new(i2c: I2C) -> Result<Self, Error> {
        Self::build(I2CDisplayInterface::new(i2c))
    }

    /// Initialise a panel at an explicit address, for modules strapped to
    /// [`ALTERNATE_ADDRESS`].
    pub fn with_address(i2c: I2C, address: u8) -> Result<Self, Error> {
        Self::build(I2CDisplayInterface::new_custom_address(i2c, address))
    }

    fn build(interface: I2CInterface<I2C>) -> Result<Self, Error> {
        let mut display =
            Ssd1306::new(interface, DisplaySize128x64, DisplayRotation::Rotate0)
                .into_buffered_graphics_mode();
        display.init()?;
        let mut screen = Self {
            display,
            style: MonoTextStyle::new(&FONT_6X10, BinaryColor::On),
        };
        // Powering on leaves the panel's RAM in an undefined state, so it shows confetti until
        // something overwrites it. Start blank instead.
        screen.clear();
        screen.flush()?;
        Ok(screen)
    }

    /// Blank the framebuffer. Takes effect on the next [`flush`](Self::flush).
    pub fn clear(&mut self) {
        self.display.clear_buffer();
    }

    /// Draw `text` at the left edge of `row`, in rows of the 6x10 font.
    ///
    /// Rows run 0..[`ROWS`](crate::layout::ROWS); anything lower is clamped onto the last row so
    /// the text cannot silently vanish off-screen.
    pub fn line(&mut self, row: u8, text: &str) -> Result<(), Error> {
        self.text_at(0, layout::row_y(row), text)
    }

    /// Draw `text` horizontally centred on `row`.
    pub fn centered(&mut self, row: u8, text: &str) -> Result<(), Error> {
        let x = layout::centered_x(text.chars().count());
        self.text_at(x, layout::row_y(row), text)
    }

    fn text_at(&mut self, x: i32, y: i32, text: &str) -> Result<(), Error> {
        Text::with_baseline(text, Point::new(x, y), self.style, Baseline::Top)
            .draw(&mut self.display)
            .map_err(Error)?;
        Ok(())
    }

    /// Run arbitrary `embedded-graphics` drawing against the framebuffer.
    ///
    /// Use this for anything the helpers don't cover - shapes, images, other fonts.
    pub fn draw<F>(&mut self, f: F) -> Result<(), Error>
    where
        F: FnOnce(&mut Display<I2C>) -> Result<(), DisplayError>,
    {
        f(&mut self.display).map_err(Error)
    }

    /// Push the framebuffer to the panel. Until this runs, nothing drawn is visible.
    pub fn flush(&mut self) -> Result<(), Error> {
        self.display.flush().map_err(Error)
    }

    /// Set panel brightness.
    pub fn set_brightness(&mut self, brightness: Brightness) -> Result<(), Error> {
        self.display.set_brightness(brightness).map_err(Error)
    }

    /// Turn the panel on or off without discarding the framebuffer.
    pub fn set_on(&mut self, on: bool) -> Result<(), Error> {
        self.display.set_display_on(on).map_err(Error)
    }

    /// Borrow the underlying driver, for anything this wrapper doesn't expose.
    pub fn display(&mut self) -> &mut Display<I2C> {
        &mut self.display
    }
}
