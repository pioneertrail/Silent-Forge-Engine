# Silent Forge Engine

A lightweight, cross-platform 3D rendering engine built with modern OpenGL.

## Overview

Silent Forge Engine is a streamlined OpenGL-based rendering engine designed for simplicity, performance, and extensibility. It provides a solid foundation for 3D graphics applications including games, visualizations, and simulations.

## Features

- Modern OpenGL 3.3 Core Profile
- Cross-platform support (Windows, Linux, macOS)
- Modular architecture with clean separation of concerns
- Window management and input handling via GLFW
- Shader compilation and management system
- Mesh creation and rendering capabilities

## Getting Started

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler
- OpenGL drivers

### Building

1. Clone the repository:
```bash
git clone https://github.com/pioneertrail/Silent-Forge-Engine.git
cd Silent-Forge-Engine
```

2. Create a build directory and run CMake:
```bash
mkdir build && cd build
cmake .. -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF
cmake --build .
```

3. Run the demo:
```bash
./bin/NewEngine  # Linux/macOS
./bin/Debug/NewEngine.exe  # Windows
```

## Dependencies

- GLAD - OpenGL Loader-Generator
- GLFW - Cross-platform window and input management
- GLM - Mathematics library for OpenGL

## License

[MIT License](LICENSE)

## Roadmap

- Input handling system
- Camera and transform system
- Texture loading and rendering
- Lighting and material system
- Scene graph and entity management
- Physics integration
- Audio support 