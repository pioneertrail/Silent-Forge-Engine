# SilentForge Engine Examples

This directory contains example projects that demonstrate how to use various features of the SilentForge Engine.

## Example Structure

Each example is contained in its own subdirectory and demonstrates a specific feature or concept of the engine.

## Planned Examples

The following examples will be implemented once the core engine compilation issues are resolved:

1. **Basic Window** - Simple example showing how to create a window and handle basic events
2. **Triangle Rendering** - Basic rendering example with shaders
3. **Texture Loading** - Demonstrates loading and rendering textures
4. **Model Loading** - Shows how to load and render 3D models
5. **Instanced Rendering** - Example of efficient instanced rendering
6. **Input Handling** - Demonstrates handling keyboard, mouse, and gamepad input
7. **Physics Integration** - Basic physics simulation
8. **Scene Graph** - Working with the scene graph system
9. **Audio Playback** - Playing sounds and music
10. **UI Elements** - Creating and interacting with UI elements

## How to Build Examples

Once implemented, each example can be built using the following commands:

```bash
cd examples/<example_directory>
mkdir build
cd build
cmake ..
cmake --build .
```

## Running Examples

After building, the examples can be run from the `build/bin` directory.

## Contributing

Feel free to contribute your own examples by creating a pull request. Please follow the existing example structure and include clear documentation. 