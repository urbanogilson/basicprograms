//! Debouncing for the encoder's push button.

/// Something the button did.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum ButtonEvent {
    /// The button went down and stayed down past the debounce window.
    Pressed,
    /// The button came back up.
    Released,
    /// The button has now been held for the long-press interval. Emitted once per press.
    LongPress,
}

/// Debounces a push button and recognises long presses.
///
/// Time is supplied by the caller rather than read from a clock, which keeps this testable and
/// lets it work with whatever timer the platform has.
///
/// ```
/// # use rotary_encoder::{Button, ButtonEvent};
/// let mut button = Button::new();
/// assert_eq!(button.update(true, 0), None);        // too soon to trust
/// assert_eq!(button.update(true, 10), Some(ButtonEvent::Pressed));
/// ```
#[derive(Clone, Copy, Debug)]
pub struct Button {
    /// The debounced level currently believed to be true.
    stable: bool,
    /// The most recent raw sample, which may still be settling.
    candidate: bool,
    /// When `candidate` was first seen.
    changed_at: u32,
    /// When the current press was confirmed.
    pressed_at: u32,
    long_press_sent: bool,
    debounce_ms: u32,
    long_press_ms: u32,
}

impl Button {
    /// Contacts settle well inside 5 ms on a typical encoder switch.
    pub const DEFAULT_DEBOUNCE_MS: u32 = 5;
    /// Hold time that counts as a long press.
    pub const DEFAULT_LONG_PRESS_MS: u32 = 700;

    /// A button using the default timings.
    pub const fn new() -> Self {
        Self::with_timings(Self::DEFAULT_DEBOUNCE_MS, Self::DEFAULT_LONG_PRESS_MS)
    }

    /// A button with explicit debounce and long-press intervals, in milliseconds.
    pub const fn with_timings(debounce_ms: u32, long_press_ms: u32) -> Self {
        Self {
            stable: false,
            candidate: false,
            changed_at: 0,
            pressed_at: 0,
            long_press_sent: false,
            debounce_ms,
            long_press_ms,
        }
    }

    /// Feed the current level and the time in milliseconds.
    ///
    /// `pressed` is the logical state, so a caller reading an active-low pin passes
    /// `pin.is_low()`. Returns at most one event per call.
    pub fn update(&mut self, pressed: bool, now_ms: u32) -> Option<ButtonEvent> {
        if pressed != self.candidate {
            // A new level: start the debounce window rather than trusting it yet.
            self.candidate = pressed;
            self.changed_at = now_ms;
            return None;
        }

        if pressed != self.stable {
            // `wrapping_sub` because `now_ms` wraps every ~49 days; a plain subtraction would
            // underflow exactly once and then misbehave until reboot.
            if now_ms.wrapping_sub(self.changed_at) >= self.debounce_ms {
                self.stable = pressed;
                return if pressed {
                    self.pressed_at = now_ms;
                    self.long_press_sent = false;
                    Some(ButtonEvent::Pressed)
                } else {
                    Some(ButtonEvent::Released)
                };
            }
            return None;
        }

        // Level is settled and unchanged; the only thing left to check is the hold time.
        if self.stable
            && !self.long_press_sent
            && now_ms.wrapping_sub(self.pressed_at) >= self.long_press_ms
        {
            self.long_press_sent = true;
            return Some(ButtonEvent::LongPress);
        }

        None
    }

    /// The debounced state.
    pub fn is_pressed(&self) -> bool {
        self.stable
    }
}

impl Default for Button {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Feed `(pressed, now_ms)` samples, collecting every event emitted.
    fn feed(b: &mut Button, samples: &[(bool, u32)]) -> Vec<ButtonEvent> {
        samples
            .iter()
            .filter_map(|&(p, t)| b.update(p, t))
            .collect()
    }

    /// Hold `pressed` from `from` to `to` inclusive, sampling every millisecond.
    fn hold(pressed: bool, from: u32, to: u32) -> Vec<(bool, u32)> {
        (from..=to).map(|t| (pressed, t)).collect()
    }

    #[test]
    fn a_real_press_emits_exactly_one_pressed() {
        let mut b = Button::new();
        let events = feed(&mut b, &hold(true, 0, 50));
        assert_eq!(events, vec![ButtonEvent::Pressed]);
        assert!(b.is_pressed());
    }

    #[test]
    fn press_and_release_round_trip() {
        let mut b = Button::new();
        let mut seq = hold(true, 0, 50);
        seq.extend(hold(false, 51, 100));
        assert_eq!(
            feed(&mut b, &seq),
            vec![ButtonEvent::Pressed, ButtonEvent::Released]
        );
        assert!(!b.is_pressed());
    }

    #[test]
    fn bounces_shorter_than_the_window_are_suppressed() {
        // Contact chatter: flipping every millisecond never settles, so nothing is reported.
        let mut b = Button::new();
        let seq: Vec<_> = (0..40).map(|t| (t % 2 == 0, t)).collect();
        assert_eq!(feed(&mut b, &seq), vec![]);
        assert!(!b.is_pressed());
    }

    #[test]
    fn a_bounce_then_a_settled_press_reports_once() {
        let mut b = Button::new();
        let mut seq = vec![(true, 0), (false, 1), (true, 2), (false, 3), (true, 4)];
        seq.extend(hold(true, 5, 60));
        assert_eq!(feed(&mut b, &seq), vec![ButtonEvent::Pressed]);
    }

    #[test]
    fn long_press_fires_once_and_only_once() {
        let mut b = Button::new();
        let events = feed(&mut b, &hold(true, 0, 3000));
        assert_eq!(
            events,
            vec![ButtonEvent::Pressed, ButtonEvent::LongPress],
            "holding must not repeat LongPress"
        );
    }

    #[test]
    fn a_short_click_never_reports_a_long_press() {
        let mut b = Button::new();
        let mut seq = hold(true, 0, 100);
        seq.extend(hold(false, 101, 200));
        let events = feed(&mut b, &seq);
        assert!(!events.contains(&ButtonEvent::LongPress));
    }

    #[test]
    fn a_second_press_can_long_press_again() {
        let mut b = Button::new();
        let mut seq = hold(true, 0, 1000);
        seq.extend(hold(false, 1001, 1100));
        seq.extend(hold(true, 1101, 2200));
        let events = feed(&mut b, &seq);
        assert_eq!(
            events,
            vec![
                ButtonEvent::Pressed,
                ButtonEvent::LongPress,
                ButtonEvent::Released,
                ButtonEvent::Pressed,
                ButtonEvent::LongPress,
            ]
        );
    }

    #[test]
    fn timestamps_that_wrap_still_measure_correctly() {
        // The regression this guards: `now_ms - changed_at` underflows across the u32 boundary,
        // which happens once every ~49 days of uptime and then breaks the button until reboot.
        let start = u32::MAX - 10;
        let mut b = Button::new();
        let seq: Vec<_> = (0..40).map(|i| (true, start.wrapping_add(i))).collect();
        assert_eq!(feed(&mut b, &seq), vec![ButtonEvent::Pressed]);

        // And the long press, which spans the wrap by a wider margin.
        let mut b = Button::new();
        let seq: Vec<_> = (0..1000).map(|i| (true, start.wrapping_add(i))).collect();
        assert_eq!(
            feed(&mut b, &seq),
            vec![ButtonEvent::Pressed, ButtonEvent::LongPress]
        );
    }

    #[test]
    fn custom_timings_are_respected() {
        let mut b = Button::with_timings(20, 100);
        // Settled, but not yet past the longer debounce window.
        assert_eq!(feed(&mut b, &hold(true, 0, 15)), vec![]);
        assert_eq!(feed(&mut b, &hold(true, 16, 25)), vec![ButtonEvent::Pressed]);
        assert_eq!(
            feed(&mut b, &hold(true, 26, 200)),
            vec![ButtonEvent::LongPress]
        );
    }
}
