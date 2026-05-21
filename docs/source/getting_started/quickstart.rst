.. _quickstart:

Quick Start
===========

Prerequisites
-------------

.. list-table::
   :header-rows: 1
   :widths: 25 20 55

   * - Tool
     - Version
     - Notes
   * - CMake
     - ≥ 3.24
     - cmake.org or system package
   * - Qt6
     - ≥ 6.4
     - ``apt install qt6-base-dev qt6-charts-dev`` (Ubuntu)
       or Qt Online Installer
   * - Ninja
     - any
     - ``apt install ninja-build`` — optional but recommended
   * - C++ compiler
     - C++17
     - GCC ≥ 11, Clang ≥ 14, MSVC 2022

Clone and build
---------------

.. code-block:: bash

   # 1. Clone with the engine submodule
   git clone --recurse-submodules \
       git@github.com:ldv-1000111/fmcw-thz-radar-gui.git
   cd fmcw-thz-radar-gui

   # If you forgot --recurse-submodules:
   git submodule update --init --recursive

   # 2. Configure
   cmake --preset default

   # 3. Build
   cmake --build build --parallel

   # 4. Run
   ./build/fmcw_gui

Expected result
---------------

A 1280 × 800 window opens with:

- **Left dock** — parameter panel (radar system + target controls)
- **Central area** — two-tab plot (IF signal / range spectrum)
- **Status bar** — live derived quantities (f₀, Δr, f_beat, f_D)

Changing any spin box triggers an immediate recompute and plot refresh.

Updating the engine
-------------------

.. code-block:: bash

   git submodule update --remote extern/fmcw-thz-radar-sim
   cmake --build build --parallel
