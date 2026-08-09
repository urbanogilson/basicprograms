//! Per-part presets.
//!
//! Adding an amplifier or DAC means implementing [`AmpPart`] for a new marker type and exposing a
//! type alias for it next to [`crate::Max98357a`] — no driver changes.

use esp_hal::i2s::master::{Config, DataFormat};

/// The framing details that differ between I2S amplifiers and DACs.
pub trait AmpPart {
    /// Slot width and data width to clock out.
    const DATA_FORMAT: DataFormat;
    /// Framing, before the sample rate is applied.
    fn base_config() -> Config;

    /// Whether the part locks to this word-select frequency.
    ///
    /// Many parts accept only certain bands rather than any rate, and feeding one an out-of-band
    /// rate produces noise rather than a clean failure - so this is checked at build time. The
    /// default accepts everything; override it for parts with a published table.
    fn supports_sample_rate(_hz: u32) -> bool {
        true
    }
}

/// Analog Devices MAX98357A I2S class-D amplifier.
///
/// I2S Philips framing, 32-bit slots, and no MCLK input.
///
/// The part is mono and its `SD` pin picks Left, Right or `(L+R)/2`. The driver writes the same
/// sample into both slots so playback is correct however `SD` is wired.
///
/// # Slot width: use 32-bit slots
///
/// The datasheet (Digital Audio Interface table) allows BCLK/LRCLK ratios of 32, 48 and 64 with
/// BCLK anywhere in 0.2432-25.804 MHz, so 16-bit slots at 16 kHz (a 32x ratio, 512 kHz) are legal
/// on paper.
///
/// **They do not work in practice.** Measured on hardware: with
/// [`DataFormat::Data16Channel16`] the amplifier output is noise, even though the ESP32 side
/// clocks out correct frames at exactly the right rate. Switching to 32-bit slots — the 64x
/// ratio, 1.024 MHz at 16 kHz — plays cleanly. That matches what essentially every working
/// ESP-IDF and Arduino example uses.
///
/// The driver left-justifies each sample into the wider slot automatically, which matters: I2S is
/// MSB-first, so an unshifted 16-bit sample sitting in the low half of a 32-bit slot plays as
/// near-silence.
///
/// # Sample rate: only four bands
///
/// The datasheet's LRCLK ranges are **not** a continuous 8-96 kHz span - they are four discrete
/// bands, and the gaps are real. 22.05 kHz, a common choice, falls in the hole between Range 2 and
/// Range 3 and will not lock:
///
/// | Range | LRCLK (min-max) | Usable rates      |
/// |-------|-----------------|-------------------|
/// | 1     | 7.6 - 8.4 kHz   | 8000              |
/// | 2     | 15.2 - 16.8 kHz | 16000             |
/// | 3     | 30.4 - 50.4 kHz | 32000/44100/48000 |
/// | 4     | 83.8 - 100.8 kHz| 88200/96000       |
///
/// [`Max98357a::supports_sample_rate`] encodes this, and `build` rejects a rate outside it rather
/// than letting you find out by ear.
#[derive(Clone, Copy, Debug)]
pub struct Max98357a;

impl AmpPart for Max98357a {
    const DATA_FORMAT: DataFormat = DataFormat::Data32Channel32;

    fn base_config() -> Config {
        // `new_tdm_philips` is MSB shift + active-low WS, i.e. I2S Philips framing.
        Config::new_tdm_philips()
    }

    fn supports_sample_rate(hz: u32) -> bool {
        // Datasheet "Digital Audio Interface": LRCLK Ranges 1-4, min to max.
        const BANDS: [(u32, u32); 4] = [
            (7_600, 8_400),
            (15_200, 16_800),
            (30_400, 50_400),
            (83_800, 100_800),
        ];
        BANDS.iter().any(|&(lo, hi)| (lo..=hi).contains(&hz))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn max98357a_accepts_the_documented_rates() {
        for hz in [8_000, 16_000, 32_000, 44_100, 48_000, 88_200, 96_000] {
            assert!(Max98357a::supports_sample_rate(hz), "{hz} should be usable");
        }
    }

    #[test]
    fn max98357a_rejects_rates_in_the_gaps() {
        // 22050 is the trap: a common rate that sits between Range 2 and Range 3.
        for hz in [22_050, 11_025, 24_000, 64_000, 4_000, 192_000] {
            assert!(
                !Max98357a::supports_sample_rate(hz),
                "{hz} is outside every LRCLK band and must be rejected"
            );
        }
    }
}
