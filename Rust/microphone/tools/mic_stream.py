"""Frame parser for the ESP32-S3 INMP441 USB audio stream.

Wire format, little-endian throughout:

    offset  size  field
    0       4     magic "MIC1"
    4       2     seq, wraps
    6       2     payload length in bytes
    8       N     PCM samples, i16 mono

Only depends on pyserial, which is already installed system-wide.
"""

import glob

import serial

MAGIC = b"MIC1"
HEADER = 8
SAMPLE_RATE = 16000
MAX_PAYLOAD = 4096


def find_port():
    """Prefer the stable by-id path, fall back to the raw device node."""
    for pattern in ("/dev/serial/by-id/*Espressif*", "/dev/ttyACM*"):
        matches = sorted(glob.glob(pattern))
        if matches:
            return matches[0]
    raise SystemExit(
        "no ESP32-S3 serial port found - is the board plugged in? "
        "(looked for /dev/serial/by-id/*Espressif* and /dev/ttyACM*)"
    )


def open_port(port=None):
    """Open the CDC port without touching DTR/RTS.

    The S3's USB-Serial-JTAG uses those lines for its download-mode reset sequence, so letting
    pyserial assert them on open can reboot the board out from under us.
    """
    ser = serial.Serial()
    ser.port = port or find_port()
    ser.baudrate = 115200  # ignored by CDC, but pyserial wants a value
    ser.timeout = 0.1
    ser.dtr = False
    ser.rts = False
    ser.open()
    return ser


def frames(ser):
    """Yield (seq, payload_bytes), resynchronising on the magic after any corruption.

    A candidate frame is only accepted once the *next* frame's magic is visible right where its
    length says it should be. Without that check a frame the firmware abandoned mid-write (which
    it does whenever this end stops reading) would swallow the bytes that follow it, and the
    magic alone is no help because those four bytes occur naturally in PCM data. The cost is one
    frame of lookahead latency, i.e. 16 ms.
    """
    buf = bytearray()
    while True:
        chunk = ser.read(max(1, ser.in_waiting))
        if chunk:
            buf += chunk

        while True:
            start = buf.find(MAGIC)
            if start < 0:
                # Keep the last few bytes in case a magic straddles this read boundary.
                del buf[: max(0, len(buf) - (len(MAGIC) - 1))]
                break
            if len(buf) - start < HEADER:
                del buf[:start]
                break

            seq = int.from_bytes(buf[start + 4 : start + 6], "little")
            length = int.from_bytes(buf[start + 6 : start + 8], "little")
            if length == 0 or length % 2 or length > MAX_PAYLOAD:
                # Magic bytes that happened to occur inside PCM data - skip past them.
                del buf[: start + 1]
                continue

            end = start + HEADER + length
            if len(buf) < end + len(MAGIC):
                del buf[:start]
                break
            if buf[end : end + len(MAGIC)] != MAGIC:
                del buf[: start + 1]
                continue

            payload = bytes(buf[start + HEADER : end])
            del buf[:end]
            yield seq, payload


class DropCounter:
    """Turns sequence-number gaps into a count of frames the firmware couldn't send."""

    def __init__(self):
        self.dropped = 0
        self.received = 0
        self._expect = None

    def update(self, seq):
        self.received += 1
        if self._expect is not None and seq != self._expect:
            self.dropped += (seq - self._expect) % 65536
        self._expect = (seq + 1) % 65536
