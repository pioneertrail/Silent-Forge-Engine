# Architecture Decisions

**In this document:**
- [Core Design Principles](#core-design-principles)
- [Component-Based Architecture](#component-based-architecture)
- [Rendering System](#rendering-system)
- [Memory Management](#memory-management)
- [Multithreading Approach](#multithreading-approach)
- [Future Expansion](#future-expansion)

## Core Design Principles

The SilentForge Engine was designed with the following principles in mind:

1. **Simplicity over complexity** - We favor clear, maintainable code over excessive abstraction.
2. **Performance-conscious design** - Critical paths are optimized without sacrificing code clarity.
3. **Modern C++ practices** - Utilizing C++17 features for safer and more expressive code.
4. **Minimal dependencies** - Using only essential, well-maintained libraries.
5. **Thread safety by design** - Core components are designed to be thread-safe from the ground up.

### Why These Principles?

Small to medium-sized games rarely need the complexity of commercial engines like Unreal or Unity. By focusing on simplicity and performance, SilentForge provides a solid foundation without overwhelming developers with options they may never use.

## Component-Based Architecture

### Decision: Entity-Component System (ECS) with SceneNode Hierarchy

We chose a hybrid approach combining traditional scene graph hierarchies with component-based design patterns:

```
Scene
 ├── RootNode
 │    ├── Child1 (has components: Transform, Mesh, Physics)
 │    └── Child2 (has components: Transform, Light)
 └── Systems (Rendering, Physics, Audio)
```

### Alternatives Considered:

1. **Pure ECS** - Would provide better performance for large numbers of entities but sacrifices the intuitive hierarchical relationships that are natural for 3D scenes.

2. **Traditional OOP Inheritance** - While familiar, this would lead to deep inheritance trees and the "diamond problem" when entities needed multiple behaviors.

3. **Pure Data-Oriented Design** - Would maximize performance but significantly increase code complexity and reduce usability.

### Rationale:

The hybrid approach provides a good balance:

- Scene graphs are intuitive for spatial relationships and transformations
- Components allow for flexible entity behavior without deep inheritance
- Systems can operate on component types regardless of scene structure

## Rendering System

### Decision: Deferred Shading with Forward Pass Options

SilentForge uses a primarily deferred rendering pipeline with the option to use forward rendering for specific materials.

### Alternatives Considered:

1. **Forward Rendering Only** - Simpler but less efficient for many light sources.
2. **Forward+ / Clustered Forward** - Complex to implement but would provide better performance for transparent objects.
3. **Tile-Based Deferred** - Good for mobile but unnecessary complexity for desktop targets.

### Rationale:

Deferred shading was selected because:
- Handles many light sources efficiently
- Works well with physically-based rendering
- Simplifies post-processing effects
- The forward rendering pass handles transparent objects and special materials

For many games, especially those with dynamic lighting, this provides the best balance of visual quality and performance.

## Memory Management

### Decision: RAII with Smart Pointers

SilentForge uses RAII (Resource Acquisition Is Initialization) principles with smart pointers (`std::shared_ptr`, `std::unique_ptr`) rather than manual memory management or garbage collection.

### Alternatives Considered:

1. **Custom Allocators** - Would improve performance by reducing fragmentation but add complexity.
2. **Garbage Collection** - Would simplify memory management but introduce unpredictable pauses.
3. **Pool Allocators** - Good for fixed-size objects but limit flexibility.

### Rationale:

Modern C++ smart pointers provide:
- Deterministic resource cleanup
- Clear ownership semantics
- Reduced risk of memory leaks
- Compatible with standard containers and algorithms

While custom memory management would provide better performance in some cases, smart pointers offer a good balance of safety and ease of use without significant performance drawbacks for most use cases.

## Multithreading Approach

### Decision: Task-Based Parallelism

SilentForge uses a task-based approach to multithreading rather than dedicated threads for subsystems.

### Alternatives Considered:

1. **Thread-per-Subsystem** - Simpler to implement but less efficient use of cores.
2. **Job System with Fibers** - Would allow for cooperative multitasking but add complexity.
3. **Single-Threaded with Async I/O** - Simpler but would limit performance.

### Rationale:

Task-based parallelism provides:
- Better core utilization as tasks can be distributed across all available cores
- Easier load balancing
- Reduced thread synchronization overhead
- More scalable performance on systems with different core counts
- Easier to reason about than complex thread interactions

For computationally intensive operations like physics, particle systems, and AI, this approach allows for better performance scaling across different hardware.

## Future Expansion

The architecture was designed with the following expansion areas in mind:

1. **Physics Integration**
   - The component system allows for easy integration with physics libraries
   - Collision components can be added without modifying existing code

2. **Audio System**
   - Designed to be integrated as a separate subsystem 
   - Spatial audio will leverage the existing scene graph

3. **Network Replication**
   - Component-based design facilitates serialization
   - Scene graph provides natural structure for network optimizations

4. **VR/AR Support**
   - Rendering system designed to accommodate stereo rendering
   - Input system abstraction supports diverse input methods 