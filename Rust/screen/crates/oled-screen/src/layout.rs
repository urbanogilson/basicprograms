//! Text geometry for a 128x64 panel using `FONT_6X10`.
//!
//! Plain arithmetic with no display or bus involved, so it is testable on the host.

/// Panel width in pixels.
pub const WIDTH: u32 = 128;
/// Panel height in pixels.
pub const HEIGHT: u32 = 64;

/// Advance width of one `FONT_6X10` character, including its spacing.
pub const CHAR_W: u32 = 6;
/// Line height of `FONT_6X10`.
pub const CHAR_H: u32 = 10;

/// Rows of text that fit on the panel.
pub const ROWS: u8 = (HEIGHT / CHAR_H) as u8; // 6
/// Characters that fit across one row.
pub const COLS: u8 = (WIDTH / CHAR_W) as u8; // 21

/// Top edge of `row`, in pixels.
///
/// Rows past the bottom of the panel are clamped to the last one rather than returning a
/// coordinate that draws off-screen, where the text would silently vanish.
pub fn row_y(row: u8) -> i32 {
    let row = row.min(ROWS - 1) as u32;
    (row * CHAR_H) as i32
}

/// Left edge that centres `chars` characters horizontally.
///
/// Text wider than the panel is pinned to the left edge.
///
/// Both hazards here are easy to hit and neither fails loudly in release, where
/// `overflow-checks = false` turns them into silently wrong coordinates: `WIDTH - text_w`
/// underflows when the text is wider than the panel, and `chars * CHAR_W` overflows for a very
/// large `chars`. Saturating the multiply and returning early covers both.
pub fn centered_x(chars: usize) -> i32 {
    let text_w = chars.saturating_mul(CHAR_W as usize);
    if text_w >= WIDTH as usize {
        return 0;
    }
    ((WIDTH as usize - text_w) / 2) as i32
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn panel_fits_six_rows_of_twenty_one_chars() {
        assert_eq!(ROWS, 6);
        assert_eq!(COLS, 21);
    }

    #[test]
    fn rows_are_evenly_spaced() {
        assert_eq!(row_y(0), 0);
        assert_eq!(row_y(1), 10);
        assert_eq!(row_y(5), 50);
        // The last row's text still ends inside the panel.
        assert!(row_y(ROWS - 1) + CHAR_H as i32 <= HEIGHT as i32);
    }

    #[test]
    fn rows_past_the_bottom_clamp_to_the_last() {
        assert_eq!(row_y(ROWS), row_y(ROWS - 1));
        assert_eq!(row_y(200), row_y(ROWS - 1));
        assert_eq!(row_y(u8::MAX), row_y(ROWS - 1));
    }

    #[test]
    fn centering_is_symmetric() {
        // A full row of 21 chars is 126 px wide, leaving 2 px - 1 px each side.
        assert_eq!(centered_x(COLS as usize), 1);
        // 10 chars = 60 px, so (128 - 60) / 2 = 34.
        assert_eq!(centered_x(10), 34);
        // Odd leftovers round down rather than pushing text off the right edge.
        assert_eq!(centered_x(11), 31);
    }

    #[test]
    fn empty_text_centres_at_the_middle() {
        assert_eq!(centered_x(0), (WIDTH / 2) as i32);
    }

    #[test]
    fn overlong_text_pins_to_the_left_without_underflowing() {
        // The regression this guards: unsigned arithmetic here panics instead of clamping.
        assert_eq!(centered_x(COLS as usize + 1), 0);
        assert_eq!(centered_x(1000), 0);
        assert_eq!(centered_x(usize::MAX / CHAR_W as usize), 0);
    }
}
