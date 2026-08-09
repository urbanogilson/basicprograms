#!/usr/bin/env python3
# /// script
# requires-python = ">=3.12"
# dependencies = ["pyserial", "numpy", "pyqtgraph", "PySide6"]
# ///
"""Realtime scope for the ESP32-S3 INMP441 stream: waveform, spectrum and level history.

    uv run tools/scope.py
    uv run tools/scope.py --record session.wav

uv resolves the plotting stack into a throwaway environment, so nothing is installed
system-wide - which matters here because system Python is PEP 668 managed.
"""

import argparse
import sys
import threading
import wave
from collections import deque

import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtWidgets

from mic_stream import SAMPLE_RATE, DropCounter, frames, open_port

WAVEFORM_SAMPLES = SAMPLE_RATE // 2  # 0.5 s scrolling window
FFT_SAMPLES = 2048
LEVEL_HISTORY = 600  # ~20 s at the 30 Hz redraw rate
FULL_SCALE = 32768.0


class Reader(threading.Thread):
    """Owns the serial port; hands payloads to the GUI thread through a deque."""

    daemon = True

    def __init__(self, port, record_path):
        super().__init__()
        self.queue = deque(maxlen=256)
        self.drops = DropCounter()
        self.error = None
        self.samples_written = 0
        self._port = port
        self._record_path = record_path
        self._stop = threading.Event()

    def run(self):
        wav = None
        try:
            ser = open_port(self._port)
            if self._record_path:
                wav = wave.open(self._record_path, "wb")
                wav.setnchannels(1)
                wav.setsampwidth(2)
                wav.setframerate(SAMPLE_RATE)
            for seq, payload in frames(ser):
                if self._stop.is_set():
                    break
                self.drops.update(seq)
                self.queue.append(np.frombuffer(payload, dtype="<i2"))
                if wav:
                    wav.writeframes(payload)
                    self.samples_written += len(payload) // 2
        except Exception as exc:  # surfaced in the window title
            self.error = exc
        finally:
            if wav:
                wav.close()

    def stop(self):
        self._stop.set()


class Scope(QtWidgets.QMainWindow):
    def __init__(self, reader, record_path):
        super().__init__()
        self.reader = reader
        self.record_path = record_path
        self.wave_buf = np.zeros(WAVEFORM_SAMPLES, dtype=np.float32)
        self.levels = deque([-90.0] * LEVEL_HISTORY, maxlen=LEVEL_HISTORY)
        self.window = np.hanning(FFT_SAMPLES).astype(np.float32)

        layout = pg.GraphicsLayoutWidget()
        self.setCentralWidget(layout)
        self.resize(900, 720)

        wave_plot = layout.addPlot(row=0, col=0, title="waveform (0.5 s)")
        wave_plot.setYRange(-FULL_SCALE, FULL_SCALE)
        wave_plot.showGrid(x=True, y=True, alpha=0.2)
        wave_plot.setLabel("bottom", "time", units="s")
        self.wave_curve = wave_plot.plot(pen=pg.mkPen("#4fc3f7"))
        self.wave_x = np.linspace(-WAVEFORM_SAMPLES / SAMPLE_RATE, 0, WAVEFORM_SAMPLES)

        spec_plot = layout.addPlot(row=1, col=0, title="spectrum")
        spec_plot.setLogMode(x=True, y=False)
        spec_plot.setYRange(-100, 0)
        spec_plot.showGrid(x=True, y=True, alpha=0.2)
        spec_plot.setLabel("bottom", "frequency", units="Hz")
        spec_plot.setLabel("left", "dBFS")
        self.spec_curve = spec_plot.plot(pen=pg.mkPen("#81c784"))
        # Skip DC: a log axis has nowhere to put 0 Hz.
        self.freqs = np.fft.rfftfreq(FFT_SAMPLES, 1 / SAMPLE_RATE)[1:]

        level_plot = layout.addPlot(row=2, col=0, title="level")
        level_plot.setYRange(-90, 0)
        level_plot.showGrid(x=True, y=True, alpha=0.2)
        level_plot.setLabel("left", "dBFS")
        level_plot.setLabel("bottom", "frames ago")
        self.level_curve = level_plot.plot(pen=pg.mkPen("#ffb74d"))

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(33)

    def update(self):
        if self.reader.error:
            self.setWindowTitle(f"scope - ERROR: {self.reader.error}")
            self.timer.stop()
            return

        blocks = []
        while self.reader.queue:
            blocks.append(self.reader.queue.popleft())
        if blocks:
            new = np.concatenate(blocks).astype(np.float32)
            if new.size >= WAVEFORM_SAMPLES:
                self.wave_buf = new[-WAVEFORM_SAMPLES:].copy()
            else:
                self.wave_buf = np.roll(self.wave_buf, -new.size)
                self.wave_buf[-new.size :] = new

        self.wave_curve.setData(self.wave_x, self.wave_buf)

        chunk = self.wave_buf[-FFT_SAMPLES:]
        spectrum = np.abs(np.fft.rfft(chunk * self.window))[1:]
        # Normalise so a full-scale sine reads about 0 dBFS.
        spectrum /= FFT_SAMPLES * FULL_SCALE / 4
        self.spec_curve.setData(self.freqs, 20 * np.log10(np.maximum(spectrum, 1e-7)))

        rms = float(np.sqrt(np.mean(np.square(self.wave_buf))))
        self.levels.append(20 * np.log10(max(rms, 1.0) / FULL_SCALE))
        self.level_curve.setData(np.fromiter(self.levels, dtype=np.float32))

        drops = self.reader.drops
        title = (
            f"INMP441 scope - {SAMPLE_RATE} Hz mono - "
            f"{drops.received} frames, {drops.dropped} dropped, {self.levels[-1]:.1f} dBFS"
        )
        if self.record_path:
            secs = self.reader.samples_written / SAMPLE_RATE
            title += f" - recording {self.record_path} ({secs:.1f}s)"
        self.setWindowTitle(title)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--port", help="serial port (default: autodetect)")
    ap.add_argument("--record", metavar="OUT.WAV", help="also write the stream to a wav")
    args = ap.parse_args()

    reader = Reader(args.port, args.record)
    reader.start()

    app = QtWidgets.QApplication(sys.argv)
    scope = Scope(reader, args.record)
    scope.show()
    code = app.exec()
    reader.stop()
    return code


if __name__ == "__main__":
    sys.exit(main())
