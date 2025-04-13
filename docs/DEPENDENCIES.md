# Silent Forge Engine Dependencies

This document lists all dependencies required to build and run the Silent Forge Engine.

## Core Dependencies

- **OpenGL**
  - Purpose: Graphics rendering
  - Version: 4.6 or higher
  - Installation: System package manager or vendor drivers

- **GLFW**
  - Purpose: Window and context management
  - Version: 3.3.8
  - Installation: Automatically fetched via CMake FetchContent

- **GLAD**
  - Purpose: OpenGL function loading
  - Version: Generated for OpenGL 4.6
  - Installation: Automatically generated in lib/glad

- **GLM**
  - Purpose: Mathematics library
  - Version: 0.9.9.8
  - Installation: Automatically fetched via CMake FetchContent

- **SDL2**
  - Purpose: Input handling and audio
  - Version: 2.0.20 or higher
  - Installation: vcpkg or system package manager

- **nlohmann_json**
  - Purpose: JSON parsing and serialization
  - Version: 3.11.2 or higher
  - Installation: vcpkg or system package manager

## Testing Dependencies

- **Catch2**
  - Purpose: Unit testing framework
  - Version: 3.0.1 or higher
  - Installation: vcpkg or system package manager

- **Google Test**
  - Purpose: Unit testing framework
  - Version: 1.12.1 or higher
  - Installation: vcpkg or system package manager

- **lcov (optional)**
  - Purpose: Generate test coverage reports
  - Installation: 
    - Ubuntu: `sudo apt-get install lcov`
    - macOS: `brew install lcov`
    - Windows: Not supported, use GCC or Clang on WSL

## Third-Party Libraries

- **stb**
  - Purpose: Image loading and writing
  - Version: Latest from GitHub
  - Installation: Automatically downloaded by CMake if not present in lib/stb
  - Files:
    - stb_image.h: Image loading
    - stb_image_write.h: Image writing

## Build System

- **CMake**
  - Purpose: Build system
  - Version: 3.16 or higher
  - Installation: System package manager or official installer

- **vcpkg (optional)**
  - Purpose: Package manager for C++
  - Version: Latest
  - Installation: Follow instructions at https://github.com/microsoft/vcpkg

## Compiler Support

The engine is tested with the following compilers:
- GCC 9.0 or higher
- Clang 10.0 or higher
- MSVC 2019 or higher

## Platform Support

- Windows 10/11 (x64)
- Linux (Ubuntu 20.04 or higher)
- macOS 10.15 or higher 