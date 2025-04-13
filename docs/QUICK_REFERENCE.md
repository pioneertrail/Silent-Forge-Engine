# SilentForge Quick Reference

**In this document:**
- [Build Commands](#build-commands)
- [Common Tasks](#common-tasks)
- [Configuration Reference](#configuration-reference)
- [Rendering Pipeline](#rendering-pipeline)
- [Performance Tips](#performance-tips)

## Build Commands

| Task | Windows | macOS/Linux |
|------|---------|-------------|
| Setup | `scripts\setup_dependencies.bat` | `./scripts/setup_dependencies.sh` |
| Build (Debug) | `build.bat debug` | `./build.sh debug` |
| Build (Release) | `build.bat release` | `./build.sh release` |
| Clean | `build.bat clean` | `./build.sh clean` |
| Run Tests | `ctest --output-on-failure` | `ctest --output-on-failure` |

## Common Tasks

### Creating a Scene
```cpp
#include "core/SceneNode.h"
#include "rendering/Mesh.h"

// Create root node
auto rootNode = std::make_shared<SilentForge::Core::SceneNode>("Root");

// Add child nodes
auto childNode = rootNode->createChild("Child");
childNode->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

// Add mesh to node
auto mesh = std::make_shared<SilentForge::Rendering::Mesh>("assets/models/cube.obj");
childNode->attachMesh(mesh);
```

### Setting Up a Camera
```cpp
#include "core/Camera.h"

// Create perspective camera
auto camera = std::make_shared<SilentForge::Core::Camera>(
    glm::radians(60.0f),  // FOV
    16.0f/9.0f,           // Aspect ratio
    0.1f,                 // Near plane
    1000.0f               // Far plane
);

// Position and orient camera
camera->setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
camera->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
```

### Loading and Using Shaders
```cpp
#include "rendering/Shader.h"

// Load shader from files
auto shader = std::make_shared<SilentForge::Rendering::Shader>(
    "shaders/pbr.vert", 
    "shaders/pbr.frag"
);

// Set uniforms
shader->use();
shader->setUniform("viewProjection", camera->getViewProjection());
shader->setUniform("lightPosition", glm::vec3(10.0f, 10.0f, 10.0f));
shader->setUniform("albedo", glm::vec3(0.8f, 0.2f, 0.2f));
```

### Using Instanced Rendering
```cpp
#include "rendering/InstancedMesh.h"

// Create instanced mesh (maximum 1000 instances)
auto instancedMesh = std::make_shared<SilentForge::Rendering::InstancedMesh>(
    "assets/models/tree.obj", 
    1000
);

// Add instances with different transforms
for (int i = 0; i < 100; ++i) {
    glm::mat4 transform = glm::translate(
        glm::mat4(1.0f), 
        glm::vec3(i * 2.0f, 0.0f, 0.0f)
    );
    instancedMesh->addInstance(transform);
}

// Render all instances
instancedMesh->render(camera->getViewProjection());
```

## Configuration Reference

### Engine Configuration File Format
```json
{
  "rendering": {
    "resolution": [1920, 1080],
    "vsync": true,
    "msaa": 4,
    "shadowMapSize": 2048
  },
  "physics": {
    "gravity": [0.0, -9.81, 0.0],
    "timeStep": 0.016
  },
  "debug": {
    "showFps": true,
    "logLevel": "info"
  }
}
```

## Rendering Pipeline

1. **Initialization**: 
   - Create window, initialize OpenGL
   - Load shaders and meshes
   - Setup scene graph

2. **Main Loop**:
   - Process input
   - Update scene (animations, physics)
   - Render shadow maps (if enabled)
   - Render scene to framebuffer
   - Apply post-processing
   - Present final image

## Performance Tips

- Use instanced rendering for repeating objects
- Implement frustum culling to avoid rendering off-screen objects
- Minimize state changes (texture bindings, shader switches)
- Use appropriate mesh LOD (Level of Detail) based on distance
- Pre-allocate memory for dynamic objects to avoid runtime allocations
- Profile regularly with built-in tools: `engine.enableProfiling()` 