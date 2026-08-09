#!/usr/bin/env bash
# Convert any audio file into the format the firmware plays, and report its flash cost.
#
#   tools/prepare_wav.sh input.mp3 [output.wav] [rate]
#
# Decodes with ffmpeg rather than sox: the sox build here has no mp3 handler, and ffmpeg reads
# essentially anything. Output is mono 16-bit PCM, because the MAX98357A is a mono amplifier.
set -euo pipefail

in=${1:?usage: prepare_wav.sh <input> [output.wav] [rate]}
out=${2:-"$(dirname "$0")/../firmware/assets/sound.wav"}
rate=${3:-32000}

command -v ffmpeg >/dev/null || { echo "ffmpeg is required: apt install ffmpeg" >&2; exit 1; }

# The MAX98357A locks to four discrete LRCLK bands, not a continuous range. Rates in the gaps
# (22050 is the classic trap) produce noise rather than a clean failure.
case "$rate" in
  8000|16000|32000|44100|48000|88200|96000) ;;
  *)
    echo "warning: $rate Hz is outside the MAX98357A's LRCLK bands." >&2
    echo "         Supported: 8000, 16000, 32000, 44100, 48000, 88200, 96000." >&2
    echo "         The firmware will refuse it with Error::UnsupportedSampleRate." >&2
    ;;
esac

ffmpeg -v error -i "$in" -ar "$rate" -ac 1 -c:a pcm_s16le "$out" -y

bytes=$(stat -c %s "$out")
secs=$(python3 -c "import wave;w=wave.open('$out');print('%.2f'%(w.getnframes()/w.getframerate()))")
printf '%s: %.2f MB, %s s at %s Hz mono\n' "$out" "$(echo "$bytes/1048576" | bc -l)" "$secs" "$rate"
echo "Embedded via include_bytes!, so it costs that much flash (factory partition is 15.6 MB)."
