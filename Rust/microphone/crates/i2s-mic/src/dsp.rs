//! Signal processing that is useful with almost any MEMS microphone.

/// One-pole DC blocker: `y[n] = x[n] - x[n-1] + pole * y[n-1]`.
///
/// MEMS microphones come out of power-up with a large DC offset that decays over several seconds.
/// Left alone it dominates a recording as a slow ramp and makes any level meter report the offset
/// rather than the sound. On an INMP441 the offset measured around 1600 counts.
///
/// The pole sets the corner frequency: `f = (1 - pole) * rate / (2 * PI)`, so the default 0.995
/// lands near 13 Hz at 16 kHz — below anything audible worth keeping.
///
/// ```
/// # use i2s_mic::DcBlocker;
/// let mut dc = DcBlocker::new();
/// let mut samples = [1000i16; 64];
/// dc.process(&mut samples);
/// assert!(samples[63].abs() < 1000);
/// ```
#[derive(Clone, Copy, Debug)]
pub struct DcBlocker {
    prev_in: f32,
    prev_out: f32,
    pole: f32,
}

impl DcBlocker {
    /// Corner near 13 Hz at a 16 kHz sample rate.
    pub const DEFAULT_POLE: f32 = 0.995;

    /// A blocker using [`DcBlocker::DEFAULT_POLE`].
    pub const fn new() -> Self {
        Self::with_pole(Self::DEFAULT_POLE)
    }

    /// A blocker with an explicit pole, which must be in `0.0..1.0`; values closer to 1 place the
    /// corner lower and settle more slowly.
    pub const fn with_pole(pole: f32) -> Self {
        Self {
            prev_in: 0.0,
            prev_out: 0.0,
            pole,
        }
    }

    /// Filter `samples` in place.
    pub fn process(&mut self, samples: &mut [i16]) {
        for s in samples {
            let x = *s as f32;
            let y = x - self.prev_in + self.pole * self.prev_out;
            self.prev_in = x;
            self.prev_out = y;
            // Float-to-int casts saturate, which is the behaviour we want on overshoot.
            *s = y as i16;
        }
    }

    /// Clear the filter state, as if it had just been constructed.
    pub fn reset(&mut self) {
        self.prev_in = 0.0;
        self.prev_out = 0.0;
    }
}

impl Default for DcBlocker {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn removes_a_constant_offset() {
        let mut dc = DcBlocker::new();
        let mut samples = [5000i16; 4000];
        dc.process(&mut samples);
        // The step survives briefly, then decays away.
        let tail = &samples[3000..];
        assert!(
            tail.iter().all(|&s| s.abs() < 50),
            "offset should have decayed, tail max was {}",
            tail.iter().map(|s| s.abs()).max().unwrap()
        );
    }

    #[test]
    fn preserves_an_audible_tone() {
        // A 1 kHz square wave at 16 kHz: 8 samples per period, well above the 13 Hz corner.
        let mut dc = DcBlocker::new();
        let mut samples = [0i16; 800];
        for (i, s) in samples.iter_mut().enumerate() {
            *s = if (i / 8) % 2 == 0 { 4000 } else { -4000 };
        }
        dc.process(&mut samples);
        let tail = &samples[400..];
        let peak = tail.iter().map(|s| s.abs()).max().unwrap();
        assert!(peak > 3500, "tone should survive the filter, peak was {peak}");
    }

    #[test]
    fn saturates_rather_than_wrapping() {
        // A full-scale step makes the filter overshoot past i16::MAX.
        let mut dc = DcBlocker::new();
        let mut samples = [i16::MIN; 4];
        dc.process(&mut samples);
        samples = [i16::MAX; 4];
        dc.process(&mut samples);
        assert!(
            samples.iter().all(|&s| s > 0),
            "overshoot must clamp, not wrap around to negative: {samples:?}"
        );
    }

    #[test]
    fn reset_clears_state() {
        let mut dc = DcBlocker::new();
        dc.process(&mut [30000i16; 100]);
        dc.reset();

        let mut fresh = DcBlocker::new();
        let (mut a, mut b) = ([1234i16; 16], [1234i16; 16]);
        dc.process(&mut a);
        fresh.process(&mut b);
        assert_eq!(a, b);
    }

    #[test]
    fn silence_stays_silent() {
        let mut dc = DcBlocker::new();
        let mut samples = [0i16; 128];
        dc.process(&mut samples);
        assert!(samples.iter().all(|&s| s == 0));
    }
}
