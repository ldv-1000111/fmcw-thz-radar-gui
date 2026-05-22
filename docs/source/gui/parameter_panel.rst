.. _parameter_panel:

Parameter Panel
===============

The parameter panel lives in a dockable widget on the left side of the main
window.  It exposes all ``RadarParams`` and ``Target`` fields from the engine
as labelled spin boxes.

.. note::
   The panel never performs physics calculations itself.  Every value it reads
   or writes maps directly to a field in ``RadarParams`` or ``Target`` as
   defined in ``extern/fmcw-thz-radar-sim/include/fmcw_generator.hpp``.

Radar system group
------------------

.. list-table::
   :header-rows: 1
   :widths: 20 15 15 50

   * - Control
     - Unit
     - Default
     - Notes
   * - f₀
     - GHz
     - 300
     - Centre frequency. 100–300 GHz = sub-THz; > 300 GHz = true THz.
       Maps to ``RadarParams::f0``.
   * - B
     - GHz
     - 4
     - Sweep bandwidth. Determines range resolution Δr = c/2B.
       Maps to ``RadarParams::bandwidth``.
   * - Tc
     - µs
     - 100
     - Chirp duration. Maps to ``RadarParams::chirp_time``.
   * - fs
     - MHz
     - 50
     - IF sampling rate → 5000 samples/chirp at default Tc.
       Maps to ``RadarParams::fs``.

Derived labels
--------------

Three read-only labels update instantly on every spin-box change:

- **λ** — wavelength in mm (≥ 1 mm) or µm (< 1 mm), computed as c/f₀.
- **Δr** — range resolution in cm, computed as c/2B.
- **Δv** — velocity resolution in mm/s (or µm/s at very high frequencies),
  computed as λ / (2 × N_c × T_c) where N_c = 256 chirps (fixed for Phase 1).

At default parameters (300 GHz, B = 4 GHz, T_c = 100 µs, N_c = 256):

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - Label
     - Value
     - Formula
   * - λ
     - 1.00 mm
     - c / f₀ = 3×10⁸ / 300×10⁹
   * - Δr
     - 3.75 cm
     - c / 2B = 3×10⁸ / (2 × 4×10⁹)
   * - Δv
     - 19.53 mm/s
     - λ / (2 × N_c × T_c) = 0.001 / (2 × 256 × 100×10⁻⁶)

These are display-only. The authoritative values are computed by
``RadarEngine`` from the same formulas via ``rangeResM()``,
``velResMps()``, and ``wavelengthM()``.

Target group
------------

.. list-table::
   :header-rows: 1
   :widths: 20 15 15 50

   * - Control
     - Unit
     - Default
     - Notes
   * - Range
     - m
     - 50
     - Initial target range. Maps to ``Target::range``.
   * - Velocity
     - m/s
     - 0
     - Radial velocity. Positive = approaching. Maps to ``Target::velocity``.
   * - Vib amp
     - mm
     - 0.2
     - Micro-Doppler vibration amplitude. Maps to ``Target::vib_amp``
       (stored as metres internally).
   * - Vib freq
     - Hz
     - 200
     - Vibration frequency. 200 Hz = engine idle; 2.5 Hz = pedestrian limb;
       80 Hz = drone rotor. Maps to ``Target::vib_freq``.

Signal flow
-----------

.. code-block:: text

   User edits spin box
         ↓
   QDoubleSpinBox::valueChanged
         ↓
   ParameterPanel::onAnyControlChanged()
         ↓
   emit paramsChanged()
         ↓
   MainWindow::onParamsChanged()
         ↓
   RadarEngine::setParams() / setTarget()
         ↓
   RadarEngine::compute()   →   generate_chirp_if()
         ↓
   emit signalReady()
         ↓
   PlotWidget::updateIfSignal() / updateSpectrum()

.. todo::
   Add screenshot of the parameter panel once the first build is deployed.
