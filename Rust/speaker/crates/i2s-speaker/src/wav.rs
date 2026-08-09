//! A minimal `no_std` RIFF/WAVE parser that borrows rather than copies.
//!
//! Scope is deliberately narrow: 16-bit PCM, mono or stereo. Anything else is reported as
//! [`WavError::Unsupported`] rather than played as noise.

/// Bytes in a chunk header: four-character code plus a `u32` size.
const CHUNK_HEADER: usize = 8;

/// Why a byte slice could not be read as a playable WAV.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum WavError {
    /// The file does not start with a `RIFF` chunk.
    NotRiff,
    /// The `RIFF` chunk is not of form type `WAVE`.
    NotWave,
    /// No `fmt ` chunk was found.
    MissingFmt,
    /// No `data` chunk was found.
    MissingData,
    /// A chunk claims to be longer than the bytes that follow it.
    Truncated,
    /// Parsed cleanly but is not something we can play.
    Unsupported {
        /// 1 is PCM; anything else (IEEE float, A-law, extensible, ...) is rejected.
        format: u16,
        /// Bits per sample; only 16 is supported.
        bits: u16,
        /// Channel count; only 1 and 2 are supported.
        channels: u16,
    },
}

/// A parsed WAV file, borrowing its sample data from the input slice.
///
/// ```
/// # use i2s_speaker::Wav;
/// # let bytes = i2s_speaker::wav::doctest_wav();
/// let wav = Wav::parse(&bytes).unwrap();
/// assert_eq!(wav.sample_rate(), 16_000);
/// assert_eq!(wav.channels(), 1);
/// ```
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Wav<'a> {
    sample_rate: u32,
    channels: u16,
    data: &'a [u8],
}

impl<'a> Wav<'a> {
    /// Parse a WAV file held in memory, typically from `include_bytes!`.
    pub fn parse(bytes: &'a [u8]) -> Result<Self, WavError> {
        if bytes.len() < 12 {
            return Err(WavError::Truncated);
        }
        if &bytes[..4] != b"RIFF" {
            return Err(WavError::NotRiff);
        }
        if &bytes[8..12] != b"WAVE" {
            return Err(WavError::NotWave);
        }

        let mut fmt = None;
        let mut data = None;

        // Walk the chunk list rather than assuming `fmt ` is immediately followed by `data`:
        // real files carry LIST/INFO and other chunks in between.
        let mut pos = 12;
        while pos + CHUNK_HEADER <= bytes.len() {
            let id = &bytes[pos..pos + 4];
            let size = u32::from_le_bytes([
                bytes[pos + 4],
                bytes[pos + 5],
                bytes[pos + 6],
                bytes[pos + 7],
            ]) as usize;

            let body = pos + CHUNK_HEADER;
            let end = body.checked_add(size).ok_or(WavError::Truncated)?;
            if end > bytes.len() {
                return Err(WavError::Truncated);
            }

            match id {
                b"fmt " => fmt = Some(&bytes[body..end]),
                b"data" => data = Some(&bytes[body..end]),
                _ => {}
            }

            // RIFF pads odd-sized chunks to an even boundary; the pad byte is not counted
            // in the chunk size.
            pos = end + (size & 1);
        }

        let fmt = fmt.ok_or(WavError::MissingFmt)?;
        let data = data.ok_or(WavError::MissingData)?;
        if fmt.len() < 16 {
            return Err(WavError::Truncated);
        }

        let format = u16::from_le_bytes([fmt[0], fmt[1]]);
        let channels = u16::from_le_bytes([fmt[2], fmt[3]]);
        let sample_rate = u32::from_le_bytes([fmt[4], fmt[5], fmt[6], fmt[7]]);
        let bits = u16::from_le_bytes([fmt[14], fmt[15]]);

        if format != 1 || bits != 16 || !matches!(channels, 1 | 2) {
            return Err(WavError::Unsupported {
                format,
                bits,
                channels,
            });
        }

        Ok(Wav {
            sample_rate,
            channels,
            data,
        })
    }

    /// Frames per second.
    pub fn sample_rate(&self) -> u32 {
        self.sample_rate
    }

    /// 1 for mono, 2 for stereo.
    pub fn channels(&self) -> u16 {
        self.channels
    }

    /// Number of complete frames; a trailing partial frame is ignored.
    pub fn frames(&self) -> usize {
        self.data.len() / (2 * self.channels as usize)
    }

    /// Playback length in milliseconds.
    pub fn duration_ms(&self) -> u32 {
        if self.sample_rate == 0 {
            return 0;
        }
        (self.frames() as u64 * 1000 / self.sample_rate as u64) as u32
    }

    /// Iterate the file as mono samples, averaging both channels of a stereo file.
    pub fn samples(&self) -> Samples<'a> {
        Samples {
            data: self.data,
            channels: self.channels as usize,
            frame: 0,
        }
    }
}

/// Mono sample iterator returned by [`Wav::samples`].
#[derive(Clone, Debug)]
pub struct Samples<'a> {
    data: &'a [u8],
    channels: usize,
    frame: usize,
}

impl Iterator for Samples<'_> {
    type Item = i16;

    fn next(&mut self) -> Option<i16> {
        let frame_bytes = 2 * self.channels;
        let at = self.frame * frame_bytes;
        if at + frame_bytes > self.data.len() {
            return None;
        }
        self.frame += 1;

        let sample_at = |i: usize| -> i32 {
            let o = at + i * 2;
            i16::from_le_bytes([self.data[o], self.data[o + 1]]) as i32
        };

        // Averaging in i32 keeps the sum from wrapping, and matches what the MAX98357A does
        // in its default (L+R)/2 mode.
        let sum: i32 = (0..self.channels).map(sample_at).sum();
        Some((sum / self.channels as i32) as i16)
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        let total = self.data.len() / (2 * self.channels);
        let left = total.saturating_sub(self.frame);
        (left, Some(left))
    }
}

impl ExactSizeIterator for Samples<'_> {}

/// A tiny in-memory WAV, for doc examples.
#[doc(hidden)]
pub fn doctest_wav() -> [u8; 48] {
    let mut f = [0u8; 48];
    f[..4].copy_from_slice(b"RIFF");
    f[4..8].copy_from_slice(&40u32.to_le_bytes());
    f[8..12].copy_from_slice(b"WAVE");
    f[12..16].copy_from_slice(b"fmt ");
    f[16..20].copy_from_slice(&16u32.to_le_bytes());
    f[20..22].copy_from_slice(&1u16.to_le_bytes()); // PCM
    f[22..24].copy_from_slice(&1u16.to_le_bytes()); // mono
    f[24..28].copy_from_slice(&16_000u32.to_le_bytes());
    f[28..32].copy_from_slice(&32_000u32.to_le_bytes()); // byte rate
    f[32..34].copy_from_slice(&2u16.to_le_bytes()); // block align
    f[34..36].copy_from_slice(&16u16.to_le_bytes()); // bits
    f[36..40].copy_from_slice(b"data");
    f[40..44].copy_from_slice(&4u32.to_le_bytes());
    f[44..48].copy_from_slice(&[0x01, 0x00, 0xFF, 0xFF]); // 1, -1
    f
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Assemble a WAV from chunk parts, so tests can inject odd orderings and sizes.
    fn build(fmt: &[u8], chunks: &[(&[u8; 4], &[u8])]) -> Vec<u8> {
        let mut body = Vec::new();
        body.extend_from_slice(b"WAVE");
        body.extend_from_slice(b"fmt ");
        body.extend_from_slice(&(fmt.len() as u32).to_le_bytes());
        body.extend_from_slice(fmt);
        for (id, payload) in chunks {
            body.extend_from_slice(*id);
            body.extend_from_slice(&(payload.len() as u32).to_le_bytes());
            body.extend_from_slice(payload);
            if payload.len() % 2 == 1 {
                body.push(0); // RIFF pad byte
            }
        }
        let mut out = Vec::new();
        out.extend_from_slice(b"RIFF");
        out.extend_from_slice(&(body.len() as u32).to_le_bytes());
        out.extend_from_slice(&body);
        out
    }

    fn fmt_chunk(format: u16, channels: u16, rate: u32, bits: u16) -> Vec<u8> {
        let block_align = channels * bits / 8;
        let mut f = Vec::new();
        f.extend_from_slice(&format.to_le_bytes());
        f.extend_from_slice(&channels.to_le_bytes());
        f.extend_from_slice(&rate.to_le_bytes());
        f.extend_from_slice(&(rate * block_align as u32).to_le_bytes());
        f.extend_from_slice(&block_align.to_le_bytes());
        f.extend_from_slice(&bits.to_le_bytes());
        f
    }

    fn pcm(samples: &[i16]) -> Vec<u8> {
        samples.iter().flat_map(|s| s.to_le_bytes()).collect()
    }

    #[test]
    fn parses_a_mono_file() {
        let data = pcm(&[100, -100, 300]);
        let file = build(&fmt_chunk(1, 1, 16_000, 16), &[(b"data", &data)]);
        let wav = Wav::parse(&file).unwrap();
        assert_eq!(wav.sample_rate(), 16_000);
        assert_eq!(wav.channels(), 1);
        assert_eq!(wav.frames(), 3);
        assert_eq!(wav.samples().collect::<Vec<_>>(), vec![100, -100, 300]);
    }

    #[test]
    fn stereo_downmixes_to_the_average() {
        let data = pcm(&[1000, 2000, -400, 400]); // (L,R) frames
        let file = build(&fmt_chunk(1, 2, 44_100, 16), &[(b"data", &data)]);
        let wav = Wav::parse(&file).unwrap();
        assert_eq!(wav.channels(), 2);
        assert_eq!(wav.frames(), 2);
        assert_eq!(wav.samples().collect::<Vec<_>>(), vec![1500, 0]);
    }

    #[test]
    fn skips_chunks_before_data() {
        let data = pcm(&[7, 8]);
        let file = build(
            &fmt_chunk(1, 1, 8_000, 16),
            &[(b"LIST", b"INFOsomething"), (b"data", &data)],
        );
        let wav = Wav::parse(&file).unwrap();
        assert_eq!(wav.sample_rate(), 8_000);
        assert_eq!(wav.samples().collect::<Vec<_>>(), vec![7, 8]);
    }

    #[test]
    fn honours_the_pad_byte_after_an_odd_chunk() {
        // Without pad handling the parser lands one byte off and misses `data` entirely.
        let data = pcm(&[42]);
        let file = build(
            &fmt_chunk(1, 1, 16_000, 16),
            &[(b"LIST", b"odd"), (b"data", &data)],
        );
        let wav = Wav::parse(&file).unwrap();
        assert_eq!(wav.samples().collect::<Vec<_>>(), vec![42]);
    }

    #[test]
    fn ignores_a_trailing_partial_frame() {
        let file = build(&fmt_chunk(1, 2, 16_000, 16), &[(b"data", &[1, 0, 2, 0, 3])]);
        let wav = Wav::parse(&file).unwrap();
        assert_eq!(wav.frames(), 1);
        assert_eq!(wav.samples().count(), 1);
    }

    #[test]
    fn rejects_non_pcm_and_odd_bit_depths() {
        let data = pcm(&[0]);
        for (format, channels, bits) in [(3, 1, 16), (1, 1, 24), (1, 1, 8), (1, 3, 16)] {
            let file = build(&fmt_chunk(format, channels, 16_000, bits), &[(b"data", &data)]);
            assert!(
                matches!(Wav::parse(&file), Err(WavError::Unsupported { .. })),
                "format={format} channels={channels} bits={bits} should be unsupported"
            );
        }
    }

    #[test]
    fn rejects_bad_magic() {
        let mut file = build(&fmt_chunk(1, 1, 16_000, 16), &[(b"data", &pcm(&[0]))]);
        let good = file.clone();
        file[..4].copy_from_slice(b"RIFX");
        assert_eq!(Wav::parse(&file), Err(WavError::NotRiff));

        let mut file = good;
        file[8..12].copy_from_slice(b"AVI ");
        assert_eq!(Wav::parse(&file), Err(WavError::NotWave));
    }

    #[test]
    fn rejects_a_chunk_that_overruns_the_file() {
        let mut file = build(&fmt_chunk(1, 1, 16_000, 16), &[(b"data", &pcm(&[1, 2]))]);
        let len = file.len();
        // Inflate the data chunk size beyond what is actually present.
        file[len - 8..len - 4].copy_from_slice(&9999u32.to_le_bytes());
        assert_eq!(Wav::parse(&file), Err(WavError::Truncated));
    }

    #[test]
    fn rejects_missing_chunks_and_stubs() {
        let mut no_data = Vec::new();
        no_data.extend_from_slice(b"WAVE");
        no_data.extend_from_slice(b"fmt ");
        let fmt = fmt_chunk(1, 1, 16_000, 16);
        no_data.extend_from_slice(&(fmt.len() as u32).to_le_bytes());
        no_data.extend_from_slice(&fmt);
        let mut file = Vec::new();
        file.extend_from_slice(b"RIFF");
        file.extend_from_slice(&(no_data.len() as u32).to_le_bytes());
        file.extend_from_slice(&no_data);
        assert_eq!(Wav::parse(&file), Err(WavError::MissingData));

        assert_eq!(Wav::parse(b"RIFF"), Err(WavError::Truncated));
    }

    #[test]
    fn duration_matches_the_frame_count() {
        let data = pcm(&vec![0i16; 16_000]);
        let file = build(&fmt_chunk(1, 1, 16_000, 16), &[(b"data", &data)]);
        assert_eq!(Wav::parse(&file).unwrap().duration_ms(), 1000);
    }

    /// The synthetic files above are built by this test module, so they cannot catch a
    /// disagreement with what a real encoder emits. This parses the actual embedded asset.
    #[test]
    fn parses_the_real_embedded_asset() {
        let bytes = include_bytes!("../../../firmware/assets/sound.wav");
        let wav = Wav::parse(bytes).expect("the shipped asset must be playable");
        assert_eq!(wav.channels(), 1, "the amp is mono; prepare_wav.sh downmixes");
        assert_eq!(wav.frames(), wav.samples().count());
        assert!(wav.duration_ms() > 100, "asset is suspiciously short");
        assert!(
            wav.samples().any(|s| s.abs() > 1000),
            "asset looks silent - regenerate it with tools/prepare_wav.sh"
        );
    }

    #[test]
    fn size_hint_is_exact() {
        let data = pcm(&[1, 2, 3, 4]);
        let file = build(&fmt_chunk(1, 1, 16_000, 16), &[(b"data", &data)]);
        let wav = Wav::parse(&file).unwrap();
        let mut it = wav.samples();
        assert_eq!(it.len(), 4);
        it.next();
        assert_eq!(it.len(), 3);
    }
}
