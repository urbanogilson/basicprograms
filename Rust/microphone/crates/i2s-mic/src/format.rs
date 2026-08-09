//! Turning raw I2S bytes into mono samples.

/// Which half of a stereo I2S frame the microphone drives.
///
/// On the INMP441 this follows the `L/R` pin: tied to GND selects [`Slot::Left`], tied to VDD
/// selects [`Slot::Right`]. It must be tied one way or the other, never left floating.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Slot {
    /// First word of each frame.
    Left,
    /// Second word of each frame.
    Right,
}

impl Slot {
    /// Byte offset of this slot within a frame of two 32-bit words.
    const fn offset(self) -> usize {
        match self {
            Slot::Left => 0,
            Slot::Right => 4,
        }
    }
}

/// Bytes occupied by one stereo frame of two 32-bit slots.
pub const FRAME_BYTES: usize = 8;

/// Unpack interleaved 32-bit I2S frames into mono 16-bit samples.
///
/// Returns how many samples were written, which is the smaller of `out.len()` and the number of
/// whole frames in `bytes`. A partial trailing frame is ignored rather than producing a sample
/// from half-read data.
///
/// `shift` is how far right to move the 32-bit slot word to land on the top 16 bits of the
/// sample. MEMS parts like the INMP441 emit 24-bit data left-justified in the slot, i.e. the word
/// equals `sample24 << 8`, so a shift of 16 takes the most significant 16 bits.
pub fn unpack_mono(bytes: &[u8], slot: Slot, shift: u32, out: &mut [i16]) -> usize {
    let base = slot.offset();
    let mut n = 0;
    for frame in bytes.chunks_exact(FRAME_BYTES) {
        if n == out.len() {
            break;
        }
        let word = i32::from_le_bytes([
            frame[base],
            frame[base + 1],
            frame[base + 2],
            frame[base + 3],
        ]);
        out[n] = (word >> shift) as i16;
        n += 1;
    }
    n
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Build one frame from two 24-bit samples, each left-justified in its 32-bit slot.
    fn frame(left: i32, right: i32) -> [u8; FRAME_BYTES] {
        let mut f = [0u8; FRAME_BYTES];
        f[..4].copy_from_slice(&(left << 8).to_le_bytes());
        f[4..].copy_from_slice(&(right << 8).to_le_bytes());
        f
    }

    #[test]
    fn picks_the_requested_slot() {
        let bytes = frame(0x0055_00, 0x00AA_00);
        let mut out = [0i16; 1];

        assert_eq!(unpack_mono(&bytes, Slot::Left, 16, &mut out), 1);
        assert_eq!(out[0], 0x0055);

        assert_eq!(unpack_mono(&bytes, Slot::Right, 16, &mut out), 1);
        assert_eq!(out[0], 0x00AA);
    }

    #[test]
    fn shift_takes_the_top_16_bits_of_24_bit_data() {
        // Full-scale positive and negative 24-bit values.
        let bytes = [frame(0x7F_FFFF, 0), frame(-0x80_0000, 0)].concat();
        let mut out = [0i16; 2];
        assert_eq!(unpack_mono(&bytes, Slot::Left, 16, &mut out), 2);
        assert_eq!(out[0], 0x7FFF);
        assert_eq!(out[1], -0x8000);
    }

    #[test]
    fn sign_is_preserved() {
        let bytes = frame(-1000 << 8 >> 8, 0); // a small negative 24-bit sample
        let mut out = [0i16; 1];
        unpack_mono(&bytes, Slot::Left, 16, &mut out);
        assert!(out[0] < 0, "expected a negative sample, got {}", out[0]);
    }

    #[test]
    fn stops_at_the_end_of_out() {
        let bytes = [frame(1 << 8, 0), frame(2 << 8, 0), frame(3 << 8, 0)].concat();
        let mut out = [0i16; 2];
        assert_eq!(unpack_mono(&bytes, Slot::Left, 16, &mut out), 2);
        assert_eq!(out, [1, 2]);
    }

    #[test]
    fn ignores_a_partial_trailing_frame() {
        let mut bytes = frame(0x0042_00, 0).to_vec();
        bytes.extend_from_slice(&[0xFF; 5]); // half of a second frame
        let mut out = [0i16; 4];
        assert_eq!(unpack_mono(&bytes, Slot::Left, 16, &mut out), 1);
        assert_eq!(out[0], 0x0042);
    }

    #[test]
    fn empty_input_writes_nothing() {
        let mut out = [7i16; 2];
        assert_eq!(unpack_mono(&[], Slot::Left, 16, &mut out), 0);
        assert_eq!(out, [7, 7]);
    }

    #[test]
    fn empty_output_writes_nothing() {
        let bytes = frame(0x0042_00, 0);
        assert_eq!(unpack_mono(&bytes, Slot::Left, 16, &mut []), 0);
    }
}
