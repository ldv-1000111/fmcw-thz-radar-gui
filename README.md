# FMCW THz Radar GUI

[![GUI CI](https://github.com/ldv-1000111/fmcw-thz-radar-gui/actions/workflows/gui-ci.yml/badge.svg?branch=main)](https://github.com/ldv-1000111/fmcw-thz-radar-gui/actions/workflows/gui-ci.yml)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)](https://en.cppreference.com/w/cpp/17)
[![Qt6](https://img.shields.io/badge/Qt-6-green)](https://doc.qt.io/qt-6/)
[![License](https://img.shields.io/badge/license-MIT-lightgrey)](LICENSE)
[![Docs](https://readthedocs.org/projects/fmcw-thz-radar-gui/badge/?version=latest)](https://fmcw-thz-radar-gui.readthedocs.io)

**Author:** Luis Viveros · **Date:** May 2026 · **License:** MIT

A Qt6/C++ desktop application that drives the
[fmcw-thz-radar-sim](https://github.com/ldv-1000111/fmcw-thz-radar-sim)
Phase 1 physics engine through a live graphical interface.

📖 **GUI docs:** [fmcw-thz-radar-gui.readthedocs.io](https://fmcw-thz-radar-gui.readthedocs.io)  
📐 **Engine docs:** [fmcw-terahertz-radar-simulation.readthedocs.io](https://fmcw-terahertz-radar-simulation.readthedocs.io)

---

## What this is

The engine repo (`fmcw-thz-radar-sim`) is a dependency-free C++17 physics
library. This repo wraps it in a Qt6 desktop GUI — no reimplementation of
physics, no Python binding layer. The engine is consumed as a **git
submodule** and compiled directly into the application.

All radar parameters are live-editable via spin boxes. Every change triggers
an immediate recompute via `generate_chirp_if()` and a plot refresh.

---

## Quick Start

```bash
# 1. Clone with engine submodule
git clone --recurse-submodules \
    git@github.com:ldv-1000111/fmcw-thz-radar-gui.git
cd fmcw-thz-radar-gui

# 2. Configure + build
cmake --preset default
cmake --build build --parallel

# 3. Run
./build/fmcw_gui
```

### Dependencies

| Dependency | Version | Install |
|---|---|---|
| CMake | ≥ 3.24 | `apt install cmake` |
| Qt6 Base + Charts | ≥ 6.4 | `apt install qt6-base-dev qt6-charts-dev` |
| Ninja (optional) | any | `apt install ninja-build` |
| C++17 compiler | GCC ≥ 11 / Clang ≥ 14 / MSVC 2022 | system |

---

## Repository Structure

```
fmcw-thz-radar-gui/
├── extern/
│   └── fmcw-thz-radar-sim/        ← git submodule (Phase 1 engine)
│       └── include/fmcw_generator.hpp
├── include/
│   ├── RadarEngine.hpp             ← Qt wrapper around fmcw_core
│   ├── MainWindow.hpp
│   ├── ParameterPanel.hpp
│   └── PlotWidget.hpp
├── src/
│   ├── main.cpp
│   ├── RadarEngine.cpp
│   ├── MainWindow.cpp
│   ├── ParameterPanel.cpp
│   └── PlotWidget.cpp
├── ui/
│   ├── MainWindow.ui
│   └── ParameterPanel.ui
├── docs/                           ← ReadTheDocs Sphinx source
├── .github/workflows/
│   └── gui-ci.yml
├── .gitmodules
├── CMakeLists.txt
└── CMakePresets.json
```

---

## Architecture

```
Qt UI layer  (MainWindow · ParameterPanel · PlotWidget)
      ↓  Qt signals / slots
RadarEngine  (thin QObject wrapper, unit conversion, QVector output)
      ↓  direct C++ call
fmcw_core    (git submodule — generate_chirp_if(), RadarParams, Target)
```

`RadarEngine` never reimplements physics. It calls `generate_chirp_if()`
from the engine submodule and converts the output to `QVector<double>` for
QtCharts.

---

## Updating the engine

```bash
git submodule update --remote extern/fmcw-thz-radar-sim
cmake --build build --parallel
```

---

## License

MIT — see [LICENSE](LICENSE)  
Copyright (c) 2026 Luis Viveros
