.. _api:

API Reference
=============

This page documents the public C++ interface of the GUI layer.
The engine API (``RadarParams``, ``Target``, ``generate_chirp_if()``) is
documented in the engine repo at
`fmcw-terahertz-radar-simulation.readthedocs.io
<https://fmcw-terahertz-radar-simulation.readthedocs.io>`_.

RadarEngine
-----------

**Header:** ``include/RadarEngine.hpp``  
**Namespace:** ``fmcw_gui``  
**Base class:** ``QObject``

``RadarEngine`` is a thin ``QObject`` wrapper around ``fmcw_core``.
It owns the ``RadarParams`` and ``Target`` state, calls
``generate_chirp_if()``, and exposes the result as ``QVector<double>``
ready for QtCharts.

Configuration
~~~~~~~~~~~~~

.. code-block:: cpp

   void setParams(const RadarParams& p);
   void setTarget(const Target& t);

   RadarParams params() const;
   Target      target() const;

Derived scalar quantities
~~~~~~~~~~~~~~~~~~~~~~~~~

All quantities are computed directly from the current ``RadarParams``
and ``Target`` — no caching, no reimplementation of engine physics.

.. list-table::
   :header-rows: 1
   :widths: 35 65

   * - Method
     - Returns
   * - ``beatFreqHz()``
     - IF beat frequency in Hz: ``(B/Tc)·τ + 2·f0·v/c``
   * - ``dopplerHz()``
     - Doppler shift in Hz: ``2·v·f0/c``
   * - ``rangeResM()``
     - Range resolution in metres: ``c/2B``
   * - ``velResMps()``
     - Velocity resolution in m/s: ``λ / (2 × N_c × Tc)`` where N_c = ``RadarParams::num_chirps``
   * - ``maxRangeM()``
     - Maximum unambiguous range in metres: ``fs·c / 2μ``
   * - ``maxVelMps()``
     - Maximum unambiguous velocity in m/s: ``λ/4Tc``
   * - ``wavelengthM()``
     - Wavelength in metres: ``c/f0``
   * - ``chirpSlopeTHzS()``
     - Chirp slope in THz/s: ``B/Tc``

Signal computation
~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   void compute();   // calls generate_chirp_if(), emits signalReady()

   QVector<double> ifSignalReal()    const;  // real part, N samples
   QVector<double> ifSignalImag()    const;  // imaginary part
   QVector<double> timeAxisUs()      const;  // time axis in µs
   QVector<double> rangeSpectrumDb() const;  // single-sided magnitude, dB
   QVector<double> rangeAxisM()      const;  // range axis in metres

Qt signals
~~~~~~~~~~

.. code-block:: cpp

   void signalReady();                        // emitted after compute()
   void errorOccurred(const QString& msg);    // emitted on exception

ParameterPanel
--------------

**Header:** ``include/ParameterPanel.hpp``  
**Namespace:** ``fmcw_gui``  
**Base class:** ``QWidget``

.. code-block:: cpp

   void setParams(const RadarParams& p);   // push values into spin boxes
   void setTarget(const Target& t);

   RadarParams params() const;             // read current spin box state
   Target      target() const;

   // Called by MainWindow when a SliderBar slider moves.
   // Updates the matching spin box and emits paramsChanged().
   void syncFromSlider(const QString& id, double value);

   // Signal
   void paramsChanged();   // emitted on any spin-box change

SliderBar
---------

**Header:** ``include/SliderBar.hpp``  
**Namespace:** ``fmcw_gui``  
**Base class:** ``QWidget``

.. code-block:: cpp

   // Add a slider column to the bar
   void addSlider(const SliderSpec& spec);

   // Get/set physical value by slider ID string
   double value(const QString& id) const;
   void   setValue(const QString& id, double physicalVal);

   // Configure the ADAS band overlay on the f0 BandSlider
   void setAdasBand(int loInt, int hiInt, const QColor& colour);

   // Signal — emitted on every slider move
   void valueChanged(const QString& id, double physicalValue);

PlotWidget
----------

**Header:** ``include/PlotWidget.hpp``  
**Namespace:** ``fmcw_gui``  
**Base class:** ``QWidget``

.. code-block:: cpp

   void updateIfSignal(const QVector<double>& timeUs,
                       const QVector<double>& amplitude);

   void updateSpectrum(const QVector<double>& rangeM,
                       const QVector<double>& magDb);

.. rubric:: Framework references

The GUI layer uses the following frameworks. Full citations are in
:ref:`gui_bibliography`.

- **Qt6 Widgets & Charts** — ``QWidget``, ``QDockWidget``, ``QDoubleSpinBox``,
  ``QLineSeries``, ``QChartView``, ``QValueAxis`` [qt6docs]_ [qtcharts]_
- **CMake** — ``cmake_minimum_required(VERSION 3.24)``,
  ``qt_add_executable``, ``target_link_libraries`` [cmake324]_
- **Git submodules** — engine consumed via ``extern/fmcw-thz-radar-sim``
  with ``.gitmodules`` [gitsubmodules]_

.. todo::
   Wire Doxygen/Breathe to auto-generate this page from header comments
   once the project reaches v0.2.0.
