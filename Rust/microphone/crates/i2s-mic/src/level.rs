//! Running level metering, for confirming a microphone is alive without buffering audio.

/// Amplitude of one measurement window.
#[derive(Clone, Copy, Debug, PartialEq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub struct LevelStats {
    /// Root-mean-square amplitude, in sample counts.
    pub rms: f32,
    /// Largest absolute sample seen.
    pub peak: i16,
    /// [`rms`](Self::rms) expressed in full-scale decibels.
    pub dbfs: f32,
    /// How many samples the window covered.
    pub samples: usize,
}

/// Accumulates RMS and peak without storing the samples themselves.
///
/// Feed it audio with [`Level::extend`] and call [`Level::take`] when enough has accumulated;
/// `take` returns the window's statistics and resets, ready for the next one. It reports rather
/// than logs, so the caller decides where the numbers go.
///
/// ```
/// # use i2s_mic::Level;
/// let mut level = Level::new();
/// level.extend(&[1000, -1000, 1000, -1000]);
/// let stats = level.take();
/// assert_eq!(stats.peak, 1000);
/// assert_eq!(level.len(), 0);
/// ```
#[derive(Clone, Copy, Debug, Default)]
pub struct Level {
    sum_sq: i64,
    peak: i16,
    count: usize,
}

impl Level {
    /// An empty accumulator.
    pub const fn new() -> Self {
        Self {
            sum_sq: 0,
            peak: 0,
            count: 0,
        }
    }

    /// Add one sample.
    pub fn push(&mut self, sample: i16) {
        self.sum_sq += (sample as i64) * (sample as i64);
        self.peak = self.peak.max(sample.saturating_abs());
        self.count += 1;
    }

    /// Add a block of samples.
    pub fn extend(&mut self, samples: &[i16]) {
        for &s in samples {
            self.push(s);
        }
    }

    /// How many samples have accumulated since the last [`take`](Self::take).
    pub fn len(&self) -> usize {
        self.count
    }

    /// Whether no samples have accumulated yet.
    pub fn is_empty(&self) -> bool {
        self.count == 0
    }

    /// Snapshot the window and reset. An empty window reports silence.
    pub fn take(&mut self) -> LevelStats {
        let stats = if self.count == 0 {
            LevelStats {
                rms: 0.0,
                peak: 0,
                dbfs: SILENCE_DBFS,
                samples: 0,
            }
        } else {
            let rms = libm::sqrtf(self.sum_sq as f32 / self.count as f32);
            LevelStats {
                rms,
                peak: self.peak,
                dbfs: dbfs(rms),
                samples: self.count,
            }
        };
        *self = Self::new();
        stats
    }
}

/// Reported instead of negative infinity when a window contains digital silence.
pub const SILENCE_DBFS: f32 = -90.0;

/// Full-scale decibels for an RMS amplitude, floored at [`SILENCE_DBFS`].
pub fn dbfs(rms: f32) -> f32 {
    if rms < 1.0 {
        return SILENCE_DBFS;
    }
    20.0 * libm::log10f(rms / 32768.0)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn rms_of_a_square_wave_is_its_amplitude() {
        let mut level = Level::new();
        level.extend(&[3000, -3000, 3000, -3000]);
        let stats = level.take();
        assert!((stats.rms - 3000.0).abs() < 0.5, "rms was {}", stats.rms);
        assert_eq!(stats.peak, 3000);
        assert_eq!(stats.samples, 4);
    }

    #[test]
    fn peak_uses_absolute_value() {
        let mut level = Level::new();
        level.extend(&[10, -500, 20]);
        assert_eq!(level.take().peak, 500);
    }

    #[test]
    fn peak_handles_the_most_negative_sample() {
        let mut level = Level::new();
        level.push(i16::MIN); // saturating_abs, since -i16::MIN does not fit
        assert_eq!(level.take().peak, i16::MAX);
    }

    #[test]
    fn take_resets_the_window() {
        let mut level = Level::new();
        level.extend(&[1000; 8]);
        assert_eq!(level.len(), 8);
        level.take();
        assert_eq!(level.len(), 0);
        assert!(level.is_empty());
        assert_eq!(level.take().samples, 0);
    }

    #[test]
    fn empty_window_reports_silence() {
        let stats = Level::new().take();
        assert_eq!(stats.rms, 0.0);
        assert_eq!(stats.dbfs, SILENCE_DBFS);
    }

    #[test]
    fn dbfs_is_zero_at_full_scale_and_floored_at_silence() {
        assert!(dbfs(32768.0).abs() < 0.01, "{}", dbfs(32768.0));
        assert!((dbfs(3276.8) + 20.0).abs() < 0.01, "{}", dbfs(3276.8));
        assert_eq!(dbfs(0.0), SILENCE_DBFS);
        assert_eq!(dbfs(0.5), SILENCE_DBFS);
    }
}
