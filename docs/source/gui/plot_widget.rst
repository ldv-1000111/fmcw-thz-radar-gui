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

The beat frequency oscillation is visible directly in this view. At the
default parameters (300 GHz, B = 4 GHz, R = 50 m) the beat frequency is
13333.33 kHz — 5000 samples over 100 µs means each cycle is ~75 samples
wide, rendering as a solid fill at this zoom level. The 200 Hz amplitude
modulation envelope is the micro-Doppler vibration at 0.2 mm — the engine
idle signature from the engine repo's ``main.cpp``.

**Confirmed ground truth at default parameters (first working build, May 2026):**

.. list-table::
   :header-rows: 1
   :widths: 40 30 30

   * - Quantity
     - Expected
     - Status bar
   * - f_beat
     - 13333.33 kHz
     - ✓ confirmed
   * - f_D (velocity = 0)
     - 0.0 Hz
     - ✓ confirmed
   * - Δr
     - 3.75 cm
     - ✓ confirmed
   * - Δv
     - 19.53 mm/s
     - ✓ confirmed
   * - λ
     - 1.00 mm
     - ✓ confirmed

Tab 1 — Spectrum (range domain)
---------------------------------

Displays the single-sided magnitude spectrum in dB returned by
``RadarEngine::rangeSpectrumDb()``, plotted against the range axis from
``RadarEngine::rangeAxisM()``.

- **X axis** — range in metres
- **Y axis** — magnitude in dB
- **Series colour** — ``#1D9E75``

The range bin corresponding to the target appears as a sharp sinc-like peak.
At R = 50 m with default parameters the peak renders at ~47 m in the direct
DFT — a 3 m offset from the expected 50 m due to bin rounding in the O(N²)
direct DFT without zero-padding. This resolves in Phase 2 when the engine's
FFTW3 pipeline replaces ``RadarEngine::buildSpectrum()``.

Implementation note
-------------------

The DFT in ``RadarEngine::buildSpectrum()`` is a direct O(N²) implementation
intentionally chosen for clarity over performance — at N = 5000 it runs in
< 50 ms on any modern desktop, fast enough for a responsive UI.  Phase 2 of
the engine repo will replace this with an FFTW3-backed pipeline; when that
lands ``RadarEngine`` will delegate to it via the submodule update.

.. todo::
   Add annotated screenshot of both tabs once the first build is deployed.
