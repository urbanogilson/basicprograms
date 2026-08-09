//! Per-part presets.
//!
//! Adding a microphone means implementing [`MicPart`] for a new marker type and exposing a type
//! alias for it next to [`crate::Inmp441`] — no driver changes.

use esp_hal::i2s::master::{Config, DataFormat};

use crate::format::Slot;

/// The wiring and framing details that differ between I2S MEMS microphones.
pub trait MicPart {
    /// Slot width and data width the part clocks out.
    const DATA_FORMAT: DataFormat;
    /// How far right to shift a slot word to land on the top 16 bits of the sample.
    const SHIFT: u32;
    /// Which slot the part drives when its channel-select pin is tied low.
    const DEFAULT_SLOT: Slot;
    /// Framing, before the sample rate is applied.
    fn base_config() -> Config;
}

/// InvenSense INMP441 omnidirectional MEMS microphone.
///
/// 24-bit data left-justified in a 32-bit slot, I2S Philips framing, no MCLK input. Both slots
/// are read and the unused one discarded, which is more predictable than masking a slot off in
/// hardware.
#[derive(Clone, Copy, Debug)]
pub struct Inmp441;

impl MicPart for Inmp441 {
    const DATA_FORMAT: DataFormat = DataFormat::Data32Channel32;
    const SHIFT: u32 = 16;
    const DEFAULT_SLOT: Slot = Slot::Left;

    fn base_config() -> Config {
        // `new_tdm_philips` is MSB shift + active-low WS, i.e. I2S Philips framing.
        Config::new_tdm_philips()
    }
}
