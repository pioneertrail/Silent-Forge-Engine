# Silent Forge Engine

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Test Coverage](https://img.shields.io/badge/Test%20Coverage->80%25-green.svg)](docs/testing-guidelines.md)
[![Memory Safety](https://img.shields.io/badge/Memory%20Safety-Zero%20Leaks-green.svg)](docs/testing-guidelines.md)

A lightweight, cross-platform 3D rendering engine built with modern OpenGL.

## Overview

Silent Forge Engine is a streamlined OpenGL-based rendering engine designed for simplicity, performance, and extensibility. It provides a solid foundation for 3D graphics applications including games, visualizations, and simulations.

## Key Features

- Modern OpenGL 3.3 Core Profile
- Cross-platform support (Windows, Linux, macOS)
- Modular architecture with clean separation of concerns
- Efficient instanced rendering with frustum culling
- Comprehensive test coverage (>80%)
- Thread-safe core components

## Getting Started

- [Setup Guide](SETUP.md) - Environment setup and installation
- [Documentation](docs/README.md) - Complete documentation
- [Contributing](docs/CONTRIBUTING.md) - How to contribute

## Quick Start

```bash
# Clone repository with submodules
git clone --recursive https://github.com/pioneertrail/Silent-Forge-Engine.git
cd Silent-Forge-Engine

# Setup dependencies and build (Windows)
scripts\setup_dependencies.bat
build.bat

# Run demo
.\bin\Debug\SilentForgeEngine.exe
```

## Project Structure

```
SilentForge/
├── assets/      # Game assets and resources
├── include/     # Public header files
├── src/         # Source files
├── shaders/     # GLSL shader files
├── tests/       # Test suite
├── docs/        # Documentation
└── scripts/     # Build and utility scripts
```

## Dependencies

- GLFW - Window and input management
- GLM - Mathematics library
- GLAD - OpenGL loader
- STB - Image loading
- Google Test/Catch2 - Testing frameworks

## License

[MIT License](LICENSE) 