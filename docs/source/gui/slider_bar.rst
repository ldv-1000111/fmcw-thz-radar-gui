.. _slider_bar:

Slider Bar
==========

The slider bar is a horizontal strip of eight labelled sliders fixed to the
bottom of the main window. It provides fast tactile parameter sweep — drag a
slider and the plots update in real time.

Each slider column shows:

- A **short label** above (e.g. f₀, B, Range)
- The **slider** itself
- The **current value with units** below (e.g. 300.0 GHz, 4.0 GHz, 50.0 m)

.. list-table::
   :header-rows: 1
   :widths: 15 15 15 15 40

   * - ID
     - Label
     - Range
     - Scale
     - Notes
   * - f0
     - f₀
     - 10 GHz – 10 THz
     - Log
     - Centre frequency. Green ADAS band marks 100–330 GHz.
       Double-click snaps to 300 GHz.
       Value label auto-scales between GHz and THz.
   * - bw
     - B
     - 0.1 – 500 GHz
     - Linear
     - Sweep bandwidth.
   * - tc
     - Tc
     - 1 – 500 µs
     - Linear
     - Chirp duration.
   * - fs
     - fs
     - 1 – 500 MHz
     - Linear
     - IF sampling rate.
   * - range
     - Range
     - 1 – 300 m
     - Linear
     - Target range.
   * - vel
     - Vel
     - -30 – 30 m/s
     - Linear (×0.1)
     - Radial velocity. Positive = approaching.
   * - vibamp
     - Vib amp
     - 0 – 50 mm
     - Linear (×0.1)
     - Micro-Doppler vibration amplitude.
   * - vibfreq
     - Vib Hz
     - 0 – 500 Hz
     - Linear
     - Vibration frequency. 200 Hz = engine idle; 80 Hz = drone rotor.

ADAS band marker
----------------

The f₀ slider has a green overlay band spanning 100–330 GHz — the Teradar
ADAS operating range as validated in the Mustang Technology / Army Research
Labs ACTI system (300–330 GHz, cited in ``[mustang_acti]``).

Double-clicking the f₀ slider snaps it to 300 GHz, the default operating
frequency of the Phase 1 engine.

Log scale on f₀
---------------

The f₀ slider uses a logarithmic mapping so that each decade of the THz
spectrum gets equal slider travel:

- Left third  — mm-wave (10–100 GHz)
- Middle section — sub-THz (100–300 GHz, Teradar ADAS range)
- Right two-thirds — true THz (300 GHz – 10 THz)

The mapping is: ``f(v) = 10^(log10(10) + (v-1)/999 × (log10(10000)-log10(10)))``
where v ∈ [1, 1000] and f is in GHz.

Bidirectional sync
------------------

Sliders and spin boxes stay in sync at all times:

- Dragging a slider → ``SliderBar::valueChanged`` → ``MainWindow::onSliderChanged``
  → ``ParameterPanel::syncFromSlider`` → spin box updates silently
- Editing a spin box → ``ParameterPanel::paramsChanged`` → ``MainWindow::onParamsChanged``
  → ``SliderBar::setValue`` → slider updates silently

Both paths call ``RadarEngine::compute()`` exactly once per user interaction.

.. todo::
   Add annotated screenshot of the slider bar showing the ADAS band marker.
