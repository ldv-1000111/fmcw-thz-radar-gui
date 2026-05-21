.. _plot_widget:

Plot Widget
===========

``PlotWidget`` is the central widget of the main window.  It contains a
``QTabWidget`` with two tabs, both backed by ``QtCharts::QLineSeries``.

Tab 0 — IF Signal (time domain)
--------------------------------

Displays the real part of the complex analytic IF signal returned by
``RadarEngine::ifSignalReal()``.

- **X axis** — fast time in µs (0 → Tc)
- **Y axis** — normalised amplitude
- **Series colour** — ``#185FA5`` (matching engine repo plot conventions)

The beat frequency oscillation is visible directly in this view.  At the
default parameters (300 GHz, B = 4 GHz, R = 50 m) the beat period is
approximately 75 µs (f_beat ≈ 13.3 kHz).  Adding micro-Doppler vibration
(vib amp > 0) produces a visible amplitude modulation riding on the carrier.

Tab 1 — Spectrum (range domain)
---------------------------------

Displays the single-sided magnitude spectrum in dB returned by
``RadarEngine::rangeSpectrumDb()``, plotted against the range axis from
``RadarEngine::rangeAxisM()``.

- **X axis** — range in metres
- **Y axis** — magnitude in dB
- **Series colour** — ``#1D9E75``

The range bin corresponding to the target appears as a peak.  At R = 50 m
with default parameters the peak falls at bin 1334, matching the
``scripts/plot_if.py`` validation in the engine repo.

Implementation note
-------------------

The DFT in ``RadarEngine::buildSpectrum()`` is a direct O(N²) implementation
intentionally chosen for clarity over performance — at N = 5000 it runs in
< 50 ms on any modern desktop, fast enough for a responsive UI.  Phase 2 of
the engine repo will replace this with an FFTW3-backed pipeline; when that
lands ``RadarEngine`` will delegate to it via the submodule update.

.. todo::
   Add annotated screenshot of both tabs once the first build is deployed.
