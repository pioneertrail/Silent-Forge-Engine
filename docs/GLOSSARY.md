# SilentForge Engine Glossary

**In this document:**
- [Engine Concepts](#engine-concepts)
- [Rendering Terms](#rendering-terms)
- [Core Systems](#core-systems)
- [Performance Optimization](#performance-optimization)
- [Graphics Programming](#graphics-programming)

## Engine Concepts

### Component-Based Architecture
A design pattern where entities in the scene are constructed from modular, reusable components rather than through inheritance. SilentForge uses components to define behavior and properties of scene objects.

### Scene Graph
A tree-like data structure that organizes the spatial representation of a scene. In SilentForge, the scene graph is implemented through the `SceneNode` class hierarchy, allowing for parent-child relationships and hierarchical transformations.

### Resource Manager
A subsystem that handles loading, caching, and unloading of external resources such as textures, meshes, and shaders. SilentForge's resource manager prevents duplicate loading and manages memory efficiently.

### Entity-Component System (ECS)
A software architectural pattern that is focused on data composition over inheritance. SilentForge implements a hybrid approach combining traditional scene graph with component-based design.

### Task Scheduler
A system that manages the execution of parallel tasks across multiple CPU cores. SilentForge's task scheduler distributes workloads like physics, AI, and animation updates to available threads.

## Rendering Terms

### Deferred Shading
A rendering technique where geometry and material information is first rendered to a set of buffers (G-buffer), then lighting calculations are performed in a separate pass. SilentForge uses this approach for efficient handling of many light sources.

### Forward Rendering
A traditional rendering approach where lighting calculations are performed for each object as it's rendered. SilentForge uses forward rendering for transparent objects and certain special materials.

### Instanced Rendering
A technique that allows rendering many copies of the same mesh with different transforms in a single draw call. SilentForge uses instancing for efficient rendering of repeating objects like trees, particles, or debris.

### Frustum Culling
The process of determining which objects in a 3D scene are visible to the camera before rendering. SilentForge implements frustum culling to avoid rendering objects that aren't visible in the current view.

### Physically Based Rendering (PBR)
A rendering approach that aims to simulate realistic lighting by using physically accurate material properties and lighting models. SilentForge implements PBR for realistic material representation.

### G-Buffer
A collection of texture buffers used in deferred rendering to store geometric and material information for each pixel, such as position, normal, albedo, and material properties.

### Shader
A program that runs on the GPU to determine how vertices are processed (vertex shader) and how pixels are colored (fragment shader). SilentForge uses GLSL shaders compiled at runtime.

### Mesh
A collection of vertices, edges, and faces that define the shape of a 3D object. In SilentForge, meshes are managed by the `Mesh` class which handles vertex buffer objects (VBOs) and vertex array objects (VAOs).

### Texture
An image used to add detail to 3D objects. SilentForge supports various texture types including diffuse, normal, roughness, metallic, and emissive maps.

### Material
A collection of properties that define how a surface interacts with light, including textures, shader parameters, and rendering settings. SilentForge's material system is designed for PBR workflows.

## Core Systems

### SceneNode
The fundamental building block of the scene hierarchy. Each `SceneNode` can have a transformation (position, rotation, scale), attachments (like meshes or lights), and child nodes.

### Camera
A virtual camera that defines the viewpoint from which the scene is rendered. SilentForge supports perspective and orthographic camera projections.

### InputManager
The system responsible for handling user input from keyboard, mouse, and gamepads. SilentForge's input system supports customizable bindings and input mapping.

### ResourceCache
A memory cache for loaded resources that prevents redundant loading of assets. SilentForge uses time and size-based policies to manage resource lifetimes.

### ApplicationSettings
The configuration system that manages engine and application settings, allowing for customization of rendering, input, and performance options.

## Performance Optimization

### Level of Detail (LOD)
A technique that uses simplified versions of a mesh when it's far from the camera. SilentForge supports automatic LOD selection based on distance.

### Occlusion Culling
An optimization technique that identifies objects hidden behind other objects and excludes them from rendering. SilentForge implements hierarchical occlusion culling.

### Texture Streaming
A technique that loads texture mip levels on demand based on camera distance, saving memory by only loading the required detail levels.

### Spatial Partitioning
Dividing the 3D space into regions to optimize spatial queries like collision detection or visibility determination. SilentForge uses spatial grids or octrees for this purpose.

### Batching
Combining multiple similar draw calls into a single one to reduce CPU overhead. SilentForge implements both static and dynamic batching for appropriate objects.

## Graphics Programming

### Vertex Buffer Object (VBO)
A memory buffer on the GPU that stores vertex data such as positions, normals, and texture coordinates.

### Vertex Array Object (VAO)
An object that stores the state needed to supply vertex data, including VBO bindings and attribute formats.

### Uniform Buffer Object (UBO)
A buffer used to efficiently pass uniform data to shaders, particularly useful for data that's shared across multiple shader programs.

### Framebuffer Object (FBO)
An OpenGL object that allows rendering to textures instead of the screen, essential for techniques like shadow mapping, post-processing, and deferred rendering.

### Depth Testing
A rendering technique that ensures only the closest fragments to the camera are visible, implemented using a depth buffer.

### Mipmap
A sequence of textures, each a pre-calculated, optimized version of the previous texture at half the resolution, used to improve rendering quality and performance at different distances.

### Normal Mapping
A technique that simulates high-resolution surface detail by using a texture to modify surface normals, creating the illusion of detailed geometry without additional vertices.

### Shadow Mapping
A technique for rendering shadows by first rendering the scene from the light's perspective to create a depth map, then using this map to determine which fragments are in shadow. 