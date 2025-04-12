# Silent Forge Engine

A modular, minimal OpenGL-based graphics engine for Windows with cross-platform compatibility in mind.

## Features

- OpenGL rendering pipeline
- Textured mesh rendering with UV coordinates
- Text rendering with FPS counter
- Camera controls and input handling
- Shader system
- Window management via GLFW

## Recent Updates
- Added texture support to `Mesh` with UV coordinates, using STB Image to load textures
- Text renderer displays engine name and FPS counter

## Building the Project

### Prerequisites
- CMake 3.16+
- C++17 compatible compiler
- OpenGL development libraries

### Build Steps
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Running
```bash
cd build/bin
./SilentForgeEngine  # Linux/macOS
SilentForgeEngine.exe  # Windows
```

## Directory Structure
```
Silent-Forge-Engine/
├── CMakeLists.txt
├── src/
│   ├── core/
│   │   ├── WindowManager.cpp
│   │   ├── Camera.cpp
│   │   ├── InputManager.cpp
│   ├── rendering/
│   │   ├── Shader.cpp
│   │   ├── Mesh.cpp
│   │   ├── TextRenderer.cpp
│   │   ├── Texture.cpp
│   └── main.cpp
├── include/
│   ├── core/
│   │   ├── WindowManager.hpp
│   │   ├── Camera.hpp
│   │   ├── InputManager.hpp
│   ├── rendering/
│   │   ├── Shader.hpp
│   │   ├── Mesh.hpp
│   │   ├── TextRenderer.hpp
│   │   ├── Texture.hpp
├── shaders/
│   ├── simple.vert
│   ├── simple.frag
│   ├── text.vert
│   ├── text.frag
├── assets/
│   ├── fonts/
│   │   ├── consolas.png
│   │   ├── consolas.fnt
│   ├── textures/
│   │   ├── test.ppm
├── lib/  (third-party libraries)
```

## Controls
- WASD: Move camera
- Mouse: Look around
- ESC: Exit

## Dependencies
- GLFW: Window and input management
- GLM: Math library
- STB Image: Image loading

## License
MIT License 