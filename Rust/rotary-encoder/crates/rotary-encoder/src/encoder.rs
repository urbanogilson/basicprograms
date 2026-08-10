//! Bounce-tolerant quadrature decoding.

/// Which way the shaft turned.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Direction {
    /// Increasing position.
    Clockwise,
    /// Decreasing position.
    CounterClockwise,
}

/// Quadrature transition table, indexed by `(previous_state << 2) | current_state`.
///
/// The two-bit state is `(a << 1) | b`. Gray-code order means exactly one bit changes per valid
/// step, so of the 16 possible index values only 8 are legal movement:
///
/// - `+1` for the four transitions that advance clockwise
/// - `-1` for the four that advance counter-clockwise
/// - `0` for no change (same state sampled twice) and for the four *impossible* transitions where
///   both bits flip at once, which only occur when a sample is missed or a contact glitches
///
/// This is what makes the decoder bounce-tolerant without any timing: a contact chattering
/// between two adjacent states emits alternating `+1` and `-1` that cancel in the accumulator,
/// and a genuine step only completes when the accumulator reaches a full detent.
#[rustfmt::skip]
const TRANSITIONS: [i8; 16] = [
    //          to 00  to 01  to 10  to 11
    /* from 00 */   0,     1,    -1,     0,
    /* from 01 */  -1,     0,     0,     1,
    /* from 10 */   1,     0,     0,    -1,
    /* from 11 */   0,    -1,     1,     0,
];

/// Decodes S1/S2 quadrature levels into detent steps.
///
/// Feed it sampled pin levels with [`update`](Self::update); it returns a [`Direction`] only when
/// a full detent has been completed, so a bouncing contact cannot produce phantom steps.
///
/// ```
/// # use rotary_encoder::{Direction, Encoder};
/// let mut enc = Encoder::new();
/// // One clean detent: 00 -> 01 -> 11 -> 10 -> 00
/// assert_eq!(enc.update(false, false), None);
/// assert_eq!(enc.update(false, true), None);
/// assert_eq!(enc.update(true, true), None);
/// assert_eq!(enc.update(true, false), None);
/// assert_eq!(enc.update(false, false), Some(Direction::Clockwise));
/// ```
#[derive(Clone, Copy, Debug)]
pub struct Encoder {
    /// Previous two-bit state, or `None` until the first sample establishes a baseline.
    state: Option<u8>,
    accumulator: i8,
    transitions_per_detent: i8,
    transitions: i32,
}

impl Encoder {
    /// Transitions per detent on a typical EC11/V635: one full quadrature cycle per click.
    pub const DEFAULT_TRANSITIONS_PER_DETENT: i8 = 4;

    /// An encoder using [`DEFAULT_TRANSITIONS_PER_DETENT`](Self::DEFAULT_TRANSITIONS_PER_DETENT).
    pub const fn new() -> Self {
        Self::with_transitions_per_detent(Self::DEFAULT_TRANSITIONS_PER_DETENT)
    }

    /// An encoder with an explicit detent ratio.
    ///
    /// Encoders vary: some emit one full cycle per click (4), some a half cycle (2), and detentless
    /// ones are best read at 1. If one physical click reports several steps, lower this; if
    /// several clicks are needed for one step, raise it. [`transitions`](Self::transitions) is
    /// there to measure the real ratio.
    ///
    /// Values below 1 are clamped to 1, since a ratio of zero would divide by nothing and report a
    /// step on every sample.
    pub const fn with_transitions_per_detent(n: i8) -> Self {
        Self {
            state: None,
            accumulator: 0,
            transitions_per_detent: if n < 1 { 1 } else { n },
            transitions: 0,
        }
    }

    /// Feed the current S1 and S2 levels.
    ///
    /// Call this as often as you like — sampling faster than the knob moves simply returns `None`.
    /// Returns `Some` only when a complete detent has been travelled.
    pub fn update(&mut self, a: bool, b: bool) -> Option<Direction> {
        let current = ((a as u8) << 1) | (b as u8);

        // The first sample only establishes a baseline; without this, an encoder resting at any
        // state other than 00 would decode a spurious step on the very first call.
        let previous = self.state.replace(current)?;
        if previous == current {
            return None;
        }

        let delta = TRANSITIONS[((previous << 2) | current) as usize];
        if delta == 0 {
            // An impossible transition means a sample was missed or a contact glitched. Discard
            // it rather than guessing a direction, and keep the new state as the baseline.
            return None;
        }

        self.transitions += delta as i32;
        self.accumulator += delta;

        if self.accumulator >= self.transitions_per_detent {
            self.accumulator = 0;
            Some(Direction::Clockwise)
        } else if self.accumulator <= -self.transitions_per_detent {
            self.accumulator = 0;
            Some(Direction::CounterClockwise)
        } else {
            None
        }
    }

    /// Net valid transitions seen since construction or the last [`reset`](Self::reset).
    ///
    /// Divide by the detents you actually turned to measure this encoder's true ratio.
    pub fn transitions(&self) -> i32 {
        self.transitions
    }

    /// Clear all state, including the sampled baseline.
    pub fn reset(&mut self) {
        *self = Self::with_transitions_per_detent(self.transitions_per_detent);
    }
}

impl Default for Encoder {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// The four quadrature states in clockwise Gray-code order.
    const CW_CYCLE: [(bool, bool); 4] = [
        (false, false),
        (false, true),
        (true, true),
        (true, false),
    ];

    /// Feed a sequence of levels, returning every direction emitted.
    fn feed(enc: &mut Encoder, steps: &[(bool, bool)]) -> Vec<Direction> {
        steps.iter().filter_map(|&(a, b)| enc.update(a, b)).collect()
    }

    /// Levels for `detents` clockwise clicks (negative for counter-clockwise), starting from 00.
    fn turn(detents: i32) -> Vec<(bool, bool)> {
        let mut out = vec![CW_CYCLE[0]];
        let total = detents.unsigned_abs() as usize * 4;
        for i in 1..=total {
            let idx = if detents >= 0 {
                i % 4
            } else {
                (4 - i % 4) % 4
            };
            out.push(CW_CYCLE[idx]);
        }
        out
    }

    #[test]
    fn one_clean_detent_each_way() {
        let mut enc = Encoder::new();
        assert_eq!(feed(&mut enc, &turn(1)), vec![Direction::Clockwise]);

        let mut enc = Encoder::new();
        assert_eq!(feed(&mut enc, &turn(-1)), vec![Direction::CounterClockwise]);
    }

    #[test]
    fn a_full_revolution_is_twenty_detents() {
        let mut enc = Encoder::new();
        let events = feed(&mut enc, &turn(20));
        assert_eq!(events.len(), 20, "a 20-detent encoder must report 20 clicks");
        assert!(events.iter().all(|&d| d == Direction::Clockwise));
    }

    #[test]
    fn the_first_sample_never_emits_a_step() {
        // An encoder resting anywhere other than 00 must not decode a step on the first call.
        for &(a, b) in &CW_CYCLE {
            let mut enc = Encoder::new();
            assert_eq!(enc.update(a, b), None, "first sample of {:?}", (a, b));
        }
    }

    #[test]
    fn repeated_identical_samples_do_nothing() {
        // The common case: polling far faster than the knob turns.
        let mut enc = Encoder::new();
        enc.update(false, false);
        for _ in 0..1000 {
            assert_eq!(enc.update(false, false), None);
        }
        assert_eq!(enc.transitions(), 0);
    }

    #[test]
    fn bouncing_contacts_do_not_create_phantom_steps() {
        // Chatter between two adjacent states many times, then complete the detent properly.
        let mut enc = Encoder::new();
        let mut seq = vec![CW_CYCLE[0]];
        for _ in 0..25 {
            seq.push(CW_CYCLE[1]);
            seq.push(CW_CYCLE[0]);
        }
        seq.extend_from_slice(&[CW_CYCLE[1], CW_CYCLE[2], CW_CYCLE[3], CW_CYCLE[0]]);

        let events = feed(&mut enc, &seq);
        assert_eq!(events, vec![Direction::Clockwise], "bounce must cancel out");
    }

    #[test]
    fn reversing_mid_detent_nets_to_nothing() {
        let mut enc = Encoder::new();
        // Two steps forward, two back: never completes a detent in either direction.
        let seq = [
            CW_CYCLE[0], CW_CYCLE[1], CW_CYCLE[2], CW_CYCLE[1], CW_CYCLE[0],
        ];
        assert_eq!(feed(&mut enc, &seq), vec![]);
        assert_eq!(enc.transitions(), 0);
    }

    #[test]
    fn impossible_transitions_are_discarded() {
        // 00 -> 11 flips both bits at once, which cannot happen on a real encoder.
        let mut enc = Encoder::new();
        enc.update(false, false);
        assert_eq!(enc.update(true, true), None);
        assert_eq!(enc.transitions(), 0, "a both-bits jump must not count");

        // ...and the decoder resynchronises rather than getting stuck.
        assert_eq!(
            feed(&mut enc, &[CW_CYCLE[3], CW_CYCLE[0], CW_CYCLE[1], CW_CYCLE[2], CW_CYCLE[3]]),
            vec![Direction::Clockwise]
        );
    }

    #[test]
    fn back_and_forth_returns_to_the_starting_position() {
        // The hardware bounce check, in software: shake the knob and end where you began.
        let mut enc = Encoder::new();
        let mut position = 0i32;
        for _ in 0..10 {
            for d in feed(&mut enc, &turn(3)) {
                position += if d == Direction::Clockwise { 1 } else { -1 };
            }
            for d in feed(&mut enc, &turn(-3)) {
                position += if d == Direction::Clockwise { 1 } else { -1 };
            }
        }
        assert_eq!(position, 0, "position drifted: {position}");
    }

    #[test]
    fn detent_ratio_is_configurable() {
        // At 1 transition per detent every quadrature step is a click: 4x the events.
        let mut enc = Encoder::with_transitions_per_detent(1);
        assert_eq!(feed(&mut enc, &turn(1)).len(), 4);

        // At 8, one click per two full cycles: half the events.
        let mut enc = Encoder::with_transitions_per_detent(8);
        assert_eq!(feed(&mut enc, &turn(2)).len(), 1);
    }

    #[test]
    fn a_zero_or_negative_ratio_is_clamped() {
        // A ratio of 0 would otherwise report a step on every single transition.
        for n in [0, -1, i8::MIN] {
            let mut enc = Encoder::with_transitions_per_detent(n);
            assert_eq!(feed(&mut enc, &turn(1)).len(), 4);
        }
    }

    #[test]
    fn reset_clears_the_baseline_too() {
        let mut enc = Encoder::new();
        feed(&mut enc, &turn(2));
        enc.reset();
        assert_eq!(enc.transitions(), 0);
        // After a reset the next sample is a baseline again, so a clean detent still works.
        assert_eq!(feed(&mut enc, &turn(1)), vec![Direction::Clockwise]);
    }
}
