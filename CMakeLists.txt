cmake_minimum_required(VERSION 3.24)

project(fmcw_thz_radar_gui VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CMP0135: use archive extraction timestamp for FetchContent
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# ---------------------------------------------------------------------------
# Qt6
# ---------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)
qt_standard_project_setup()
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/ui")

# ---------------------------------------------------------------------------
# Compiler warnings — mirror engine repo conventions
# ---------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# ---------------------------------------------------------------------------
# Engine submodule — fmcw_generator.hpp lives here
# ---------------------------------------------------------------------------
set(ENGINE_INCLUDE_DIR
    "${CMAKE_SOURCE_DIR}/extern/fmcw-thz-radar-sim/include"
    CACHE PATH "Path to fmcw-thz-radar-sim/include")

if(NOT EXISTS "${ENGINE_INCLUDE_DIR}/fmcw_generator.hpp")
    message(FATAL_ERROR
        "Engine header not found at ${ENGINE_INCLUDE_DIR}/fmcw_generator.hpp\n"
        "Run:  git submodule update --init --recursive")
endif()

# Engine static library (compiled from the submodule src/)
add_library(fmcw_core STATIC
    extern/fmcw-thz-radar-sim/src/fmcw_generator.cpp
)
target_include_directories(fmcw_core PUBLIC "${ENGINE_INCLUDE_DIR}")
target_compile_features(fmcw_core PUBLIC cxx_std_17)

# ---------------------------------------------------------------------------
# RadarEngine — thin C++ wrapper adapting fmcw_core to Qt signals/slots
# ---------------------------------------------------------------------------
add_library(radar_engine STATIC
    src/RadarEngine.cpp
    include/RadarEngine.hpp
    )
target_include_directories(radar_engine PUBLIC include)
target_link_libraries(radar_engine PUBLIC fmcw_core Qt6::Core)

# ---------------------------------------------------------------------------
# GUI executable
# ---------------------------------------------------------------------------
qt_add_executable(fmcw_gui
    src/main.cpp
    src/MainWindow.cpp
    src/ParameterPanel.cpp
    src/PlotWidget.cpp
    ui/MainWindow.ui
    ui/ParameterPanel.ui
    include/MainWindow.hpp
    include/ParameterPanel.hpp
    include/PlotWidget.hpp
)

target_include_directories(fmcw_gui PRIVATE include)

target_link_libraries(fmcw_gui PRIVATE
    radar_engine
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
)

set_target_properties(fmcw_gui PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE    TRUE
)

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
install(TARGETS fmcw_gui DESTINATION bin)
