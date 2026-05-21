.. _architecture:

Architecture
============

The GUI repo has a strict three-layer architecture.  Each layer has one
responsibility and depends only on the layer below it.

.. code-block:: text

   ┌─────────────────────────────────────────────────┐
   │  Qt UI layer                                    │
   │  MainWindow · ParameterPanel · PlotWidget       │
   │  (QWidget, QtCharts, .ui files)                 │
   └────────────────────┬────────────────────────────┘
                        │ Qt signals / slots
   ┌────────────────────▼────────────────────────────┐
   │  RadarEngine  (src/RadarEngine.cpp)             │
   │  Thin QObject wrapper — owns RadarParams,       │
   │  Target, IF buffer, spectrum buffer             │
   │  Exposes QVector<double> for QtCharts           │
   └────────────────────┬────────────────────────────┘
                        │ direct C++ call
   ┌────────────────────▼────────────────────────────┐
   │  fmcw_core  (git submodule)                     │
   │  extern/fmcw-thz-radar-sim/                     │
   │  include/fmcw_generator.hpp                     │
   │  src/fmcw_generator.cpp                         │
   │  — generate_chirp_if()                          │
   │  — RadarParams, Target structs                  │
   └─────────────────────────────────────────────────┘

Why a submodule?
----------------

The engine is the single source of truth for all physics.  Using a submodule
means:

- A bug fix in ``fmcw_generator.cpp`` is pulled with one command
  (``git submodule update --remote``) — no copy-paste drift.
- The GUI CMakeLists.txt compiles the engine's ``.cpp`` directly into a
  static library (``fmcw_core``), so there is **no ABI boundary** and no
  serialisation overhead.
- The same engine can later be used by a headless CLI, a Python binding
  (pybind11), or a WebAssembly target without touching the GUI code.

No reimplementation rule
------------------------

``RadarEngine.cpp`` must **never** reimplement a physics equation.  If a
computation belongs in the engine, it goes in ``fmcw_generator.cpp`` in the
submodule and is exposed via ``RadarEngine``'s accessor methods.
``RadarEngine`` is allowed to:

- Convert units for display (Hz → kHz, m → cm)
- Build QtCharts-compatible ``QVector<double>`` from engine output
- Trigger recomputes in response to Qt signals

It is not allowed to:

- Compute beat frequency, range resolution, Doppler shift, or any other
  quantity defined in the `engine theory pages
  <https://fmcw-terahertz-radar-simulation.readthedocs.io/en/latest/phase1/fmcw_theory.html>`_
- Duplicate any formula from ``fmcw_generator.hpp``
