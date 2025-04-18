# Silent Forge Engine

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

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

## Project Structure

```
SilentForge/
├── assets/         # Game assets and resources
├── include/        # Public header files
│   ├── core/      # Core engine components
│   └── rendering/ # Rendering system components
├── lib/           # Third-party dependencies
├── src/           # Source files
│   ├── core/     # Core implementation
│   └── rendering/# Rendering implementation
├── shaders/       # GLSL shader files
├── tools/         # Development tools and utilities
└── workflow/      # CI/CD configuration
```

## Getting Started

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler
- OpenGL drivers
- Git

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
./bin/SilentForgeEngine  # Linux/macOS
./bin/Debug/SilentForgeEngine.exe  # Windows
```

## Dependencies

- GLAD - OpenGL Loader-Generator
- GLFW - Cross-platform window and input management
- GLM - Mathematics library for OpenGL

## Development Notes

### Current Focus
- Input handling system
- Camera and transform system
- Texture loading and rendering
- Lighting and material system
- Scene graph and entity management
- Physics integration
- Audio support

### Troubleshooting

#### Common Issues

1. **CMake Configuration Fails**
   - Ensure you have the required dependencies installed
   - Check your CMake version
   - Verify OpenGL drivers are properly installed

2. **Build Fails**
   - Check compiler compatibility
   - Ensure all dependencies are properly linked
   - Verify system OpenGL version meets requirements

3. **Runtime Errors**
   - Check shader compilation logs
   - Verify asset paths are correct
   - Ensure proper OpenGL context initialization 