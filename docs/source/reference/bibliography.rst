.. _gui_bibliography:

Bibliography
============

.. note::

   This GUI repository does not derive new physics.  All signal-model
   equations are implemented in the engine submodule
   (``extern/fmcw-thz-radar-sim``) and documented with full source
   annotations in the engine's bibliography at
   `fmcw-terahertz-radar-simulation.readthedocs.io/en/latest/reference/bibliography
   <https://fmcw-terahertz-radar-simulation.readthedocs.io/en/latest/reference/bibliography.html>`_.

   The references below cover only the Qt and C++ frameworks used by the
   GUI layer itself.

.. rubric:: Qt Framework

.. [qt6docs]
   The Qt Company (2024). *Qt 6 Documentation*.
   https://doc.qt.io/qt-6/
   *(QWidget, QDockWidget, QDoubleSpinBox, QStatusBar — used in
   MainWindow and ParameterPanel.)*

.. [qtcharts]
   The Qt Company (2024). *Qt Charts Overview — Qt 6*.
   https://doc.qt.io/qt-6/qtcharts-overview.html
   *(QLineSeries, QChartView, QValueAxis — used in PlotWidget.)*

.. rubric:: Build system

.. [cmake324]
   Kitware (2024). *CMake 3.24 Documentation*.
   https://cmake.org/cmake/help/v3.24/
   *(cmake_minimum_required, qt_add_executable, FetchContent — used in
   CMakeLists.txt.)*

.. [gitsubmodules]
   Git Project (2024). *Git Submodules*.
   https://git-scm.com/book/en/v2/Git-Tools-Submodules
   *(Submodule pattern used to consume fmcw-thz-radar-sim as a
   dependency without copying sources.)*
