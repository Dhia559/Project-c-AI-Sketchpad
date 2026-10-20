# AI Sketchpad (C++ + SFML)

A simple 2D sketchpad built with C++ and SFML. It has a toolbar with basic drawing tools (Pen, Brush, Eraser), a drawable canvas, and a small local Python script for image enhancement.

## Features
- Canvas drawing with mouse input
- Tools: Pen, Brush, Eraser
- Toolbar UI with selectable tools/buttons
- `enhance_local.py` helper for local image enhancement experiments

## Tech Stack
- C++17
- [SFML](https://www.sfml-dev.org/) (Graphics, Window, System)
- CMake
- Python 3 (optional, for `enhance_local.py`)

## Project Structure
```text
.
├── include/        # Public headers (e.g. Graphics.hpp)
├── src/            # Source files (main.cpp, Sketchpad, tools, etc.)
├── .vscode/        # VS Code configs (launch/tasks/c_cpp_properties)
└── CMakeLists.txt  # Build configuration
```

## Prerequisites
- A C++17 compiler (e.g. MSVC, MinGW/g++, Clang)
- [CMake](https://cmake.org/) >= 3.10
- SFML installed and discoverable by CMake (or set `SFML_DIR`)

## Build & Run (Windows / MinGW/MSVC)

```bash
# from repo root
mkdir build
cd build
cmake ..
cmake --build . --config Debug
# run the executable (name may vary)
./AI_Sketchpad.exe  # or the target name from CMake
```

If you're using the VS Code tasks/launch configs already present, you can also build/run from there.

## Usage
1. Launch the app
2. Select a tool from the toolbar (Pen/Brush/Eraser)
3. Draw on the canvas with your mouse
4. Switch tools/colors as needed

## Notes
- `build/` contains generated artifacts. It's recommended to not commit build outputs (see `.gitignore` below).
- `enhance_local.py` is a local utility script; it isn't required to run the app.

## License
Add a license of your choice (e.g. MIT) if you plan to share this publicly.
