#!/usr/bin/env python3
"""Record the ESP32-S3 INMP441 stream to a wav file of any length.

Needs only pyserial, which is already installed system-wide - no uv, no venv:

    python3 tools/record.py out.wav              # until Ctrl-C
    python3 tools/record.py --seconds 30 out.wav
"""

import argparse
import sys
import time
import wave

from mic_stream import SAMPLE_RATE, DropCounter, frames, open_port


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("output", help="wav file to write")
    ap.add_argument("--port", help="serial port (default: autodetect)")
    ap.add_argument(
        "--seconds", type=float, help="stop after this long (default: until Ctrl-C)"
    )
    args = ap.parse_args()

    ser = open_port(args.port)
    print(f"reading {ser.port}", file=sys.stderr)

    drops = DropCounter()
    written = 0
    started = time.monotonic()

    with wave.open(args.output, "wb") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(SAMPLE_RATE)
        try:
            for seq, payload in frames(ser):
                drops.update(seq)
                wav.writeframes(payload)
                written += len(payload) // 2

                elapsed = written / SAMPLE_RATE
                if drops.received % 32 == 0:
                    print(
                        f"\r{elapsed:7.1f}s  {written:9d} samples  "
                        f"dropped={drops.dropped}",
                        end="",
                        file=sys.stderr,
                    )
                if args.seconds and elapsed >= args.seconds:
                    break
        except KeyboardInterrupt:
            pass
        finally:
            ser.close()

    wall = time.monotonic() - started
    print(
        f"\nwrote {args.output}: {written} samples, {written / SAMPLE_RATE:.2f}s audio "
        f"in {wall:.1f}s wall, {drops.dropped} frames dropped",
        file=sys.stderr,
    )
    if written == 0:
        print(
            "no frames received - is the firmware streaming? check `cargo run --release`",
            file=sys.stderr,
        )
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
