# Troubleshooting Guide

**In this document:**
- [Multi-Agent Issues](#multi-agent-issues)
- [Gamepad Configuration Issues](#gamepad-configuration-issues)
- [Gamepad Issues](#gamepad-issues)
- [Performance Issues](#performance-issues)
- [Rendering Problems](#rendering-problems)
- [Build System Issues](#build-system-issues)
- [Diagnostic Flowcharts](#diagnostic-flowcharts)

## Multi-Agent Issues

### Overlapping File Edits
- **Symptoms**: Merge conflicts when merging PRs from Agent 1 and Agent 2.
- **Solution**:
  1. Assign distinct files/modules to each agent in the issue (e.g., Agent 1: `shadow_map.cpp`, Agent 2: `renderer.cpp`).
  2. Comment in the issue to lock files (e.g., "Agent 1 owns `shadow_map.cpp` until #125").
  3. Merge PRs sequentially, resolving conflicts manually.
- **Prevention**:
  - Define file boundaries in issue subtasks.
  - Use `git diff` to check for overlaps before assigning tasks.

### Inconsistent Code Style
- **Symptoms**: Agent 1 or Agent 2 skips `.clang-format` or deviates from C++ Core Guidelines.
- **Solution**:
  1. Run `clang-format -i src/*` on agent output.
  2. Update agent prompts to emphasize `CONTRIBUTING.md`.
  3. Reject PRs failing format checks in CI.
- **Prevention**:
  - Configure Cursor to auto-apply `.clang-format`.
  - Include style guidelines in every prompt.

### Missing Tests or Documentation
- **Symptoms**: Agent 3 fails to provide tests/docs for Agent 1 or Agent 2's changes.
- **Solution**:
  1. Review PRs for test coverage and doc updates.
  2. Task Agent 3 with backfilling tests/docs in a follow-up PR.
  3. Update Agent 3's prompt to check PR descriptions.
- **Prevention**:
  - Include test/doc requirements in issue subtasks.
  - Use CI to enforce coverage thresholds.

### Prompt Failures
- **Symptoms**: Agent output doesn't match requirements (e.g., Agent 1 skips SIMD optimization).
- **Solution**:
  1. Refine prompt with specific instructions (e.g., "Use SIMD for matrix operations").
  2. Re-run task with updated prompt.
  3. Document successful prompts in `docs/agent_prompts.md`.
- **Prevention**:
  - Test prompts on small tasks first.
  - Maintain a prompt library in `docs/agent_prompts.md`.

### Performance Regressions
- **Symptoms**: Agent 1's changes cause unexpected performance degradation.
- **Solution**:
  1. Run performance tests before and after changes.
  2. Profile critical paths using Tracy.
  3. Revert or optimize problematic changes.
- **Prevention**:
  - Include performance benchmarks in issue templates.
  - Set clear performance targets for each task.

### Cross-Platform Issues
- **Symptoms**: Agent 2's changes work on one platform but fail on others.
- **Solution**:
  1. Test changes on all target platforms.
  2. Use platform-specific CI runners.
  3. Document platform requirements clearly.
- **Prevention**:
  - Include cross-platform testing in CI.
  - Specify platform requirements in issue templates.

### Documentation Gaps
- **Symptoms**: Agent 3's documentation is incomplete or unclear.
- **Solution**:
  1. Review documentation against requirements.
  2. Request specific updates from Agent 3.
  3. Use documentation templates.
- **Prevention**:
  - Include documentation checklists in issues.
  - Set clear documentation standards.

### Integration Failures
- **Symptoms**: Changes from different agents don't work together.
- **Solution**:
  1. Run integration tests before merging.
  2. Coordinate changes through issue comments.
  3. Create integration test plans.
- **Prevention**:
  - Define clear interfaces between components.
  - Include integration testing in CI.

### CI Pipeline Failures
- **Symptoms**: CI checks fail unexpectedly (e.g., build errors, test timeouts, coverage below 80%).
- **Solution**:
  1. Review CI logs to identify the failing step (e.g., `Build`, `Test`, `Generate Coverage`).
  2. For build errors, verify CMake presets and dependencies in `.github/workflows/ci.yml`.
  3. For test failures, check test logs and rerun locally with `ctest --preset=dev --verbose`.
  4. For coverage issues, task Agent 3 with adding tests for uncovered code paths.
  5. For timeouts, optimize tests or increase CI resource limits.
- **Prevention**:
  - Run CI checks locally before pushing (`cmake --build --preset=dev`, `ctest --preset=dev`).
  - Pin dependency versions in CI (e.g., `clang-format-15`, `cmake 3.22`).
  - Monitor CI runtime and optimize long-running tests.
  - Include CI setup instructions in `docs/DEVELOPMENT_WORKFLOW.md`.

## Gamepad Configuration Issues

### Partial Configuration Loading
- **Symptoms**: Some gamepad buttons/axes don't respond despite valid configuration.
- **Cause**: The gamepad doesn't support all configured buttons/axes, leading to skipped bindings.
- **Solution**:
  1. Check logs for warnings like "Button not supported" or "Axis not supported".
  2. Use `Gamepad::isButtonSupported` and `isAxisSupported` to list supported controls.
  3. Update `gamepad.json` to include only supported buttons/axes.
  4. If no bindings load, verify gamepad connection and driver compatibility.
- **Prevention**:
  - Generate configurations dynamically based on detected gamepad capabilities.
  - Test configurations with the target gamepad model before deployment.
  - Document expected bindings in user guides.

### Unsupported Buttons/Axes
- **Symptoms**: Buttons or axes are ignored in configuration.
- **Cause**: Controls not supported by the connected gamepad.
- **Solution**:
  1. Check logs for "Button not supported" or "Axis not supported" warnings.
  2. Verify gamepad capabilities with `Gamepad::isButtonSupported` and `isAxisSupported`.
  3. Update configuration to use only supported controls.
- **Prevention**:
  - Test configuration with target gamepad before deployment.
  - Document supported controls in gamepad documentation.

### Invalid Axis Ranges
- **Symptoms**: Configuration fails to load with "Invalid axis range" error.
- **Cause**: `min` value exceeds `max` value in axis configuration.
- **Solution**:
  1. Check axis range definitions in `gamepad.json`.
  2. Ensure `min` ≤ `max` for all axes.
  3. Use default ranges (-1.0 to 1.0) if unsure.
- **Prevention**:
  - Validate axis ranges during configuration editing.
  - Include range validation in configuration tools.

### Permission Issues
- **Symptoms**: Configuration fails to load with errors like "Failed to open gamepad config".
- **Cause**: Insufficient file or directory permissions on the system.
- **Solution**:
  - **Linux**:
    1. Check permissions with `ls -l gamepad.json`.
    2. Set read permissions: `chmod 644 gamepad.json`.
    3. Verify directory access: `chmod 755 /path/to/config/dir`.
  - **macOS**:
    1. Check permissions with `ls -l gamepad.json`.
    2. Set read permissions: `chmod 644 gamepad.json`.
    3. Ensure user has access to `~/Library/Application Support/SFE`.
  - **Windows**:
    1. Check file properties for read-only status.
    2. Grant read access: `icacls gamepad.json /grant Users:R`.
    3. Verify directory access in `%APPDATA%\SFE`.
  - Ensure the user has sufficient privileges (e.g., not running as restricted user).
  - **Note**: Some commands may require sudo (Linux/macOS) or admin privileges (Windows).
- **Prevention**:
  - Set correct permissions during installation (e.g., 644 for files, 755 for directories).
  - Use platform-standard config paths (e.g., `~/.config/SFE` on Linux).
  - Document permission requirements in setup guides.

### MFi Compliance Warnings
- **Symptoms**: "Config lacks MFi compliance flag" warning on macOS.
- **Cause**: Missing MFi compliance flag in configuration.
- **Solution**:
  1. Add `"mfi_compliant": true` to configuration if using MFi controller.
  2. Set `"mfi_compliant": false` for non-MFi controllers.
  3. Update configuration to match controller type.
- **Prevention**:
  - Include MFi flag in configuration templates.
  - Document MFi requirements for macOS deployment.

### JSON Parsing Errors
- **Symptoms**: "JSON parse error" when loading configuration.
- **Cause**: Malformed JSON in configuration file.
- **Solution**:
  1. Validate JSON syntax using a JSON validator.
  2. Check for missing commas, quotes, or brackets.
  3. Ensure all values are properly formatted.
- **Prevention**:
  - Use JSON schema validation during editing.
  - Include example configurations in documentation.

## Gamepad Issues

### Gamepad Not Detected
- **Symptoms**: No gamepad detected when connected
- **Possible Causes**:
  - SDL2 game controller subsystem not initialized
  - Gamepad not recognized by SDL2
  - Driver issues
- **Solutions**:
  1. Check if SDL2 is properly initialized:
     ```cpp
     if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
         LOG_ERROR("SDL init failed: " + std::string(SDL_GetError()));
     }
     ```
  2. Verify gamepad is recognized:
     ```cpp
     for (int i = 0; i < SDL_NumJoysticks(); ++i) {
         if (SDL_IsGameController(i)) {
             LOG_INFO("Gamepad found at index " + std::to_string(i));
         }
     }
     ```
  3. Check device drivers and USB connection

### Button Mapping Issues
- **Symptoms**: Buttons not responding or mapped incorrectly
- **Possible Causes**:
  - Invalid button configuration
  - Unsupported buttons
  - Configuration file errors
- **Solutions**:
  1. Verify button configuration in JSON:
     ```json
     {
       "buttons": {
         "0": "jump",
         "1": "attack"
       }
     }
     ```
  2. Check button support:
     ```cpp
     if (!gamepad->isButtonSupported(button)) {
         LOG_WARNING("Button not supported");
     }
     ```
  3. Validate configuration file format

### Axis Issues
- **Symptoms**: Analog sticks not working or values incorrect
- **Possible Causes**:
  - Axis not calibrated
  - Deadzone issues
  - Configuration errors
- **Solutions**:
  1. Check axis values:
     ```cpp
     float value = gamepad->getAxisValue(axis);
     LOG_INFO("Axis value: " + std::to_string(value));
     ```
  2. Verify axis configuration:
     ```json
     {
       "axes": {
         "0": {"min": -1.0, "max": 1.0}
       }
     }
     ```
  3. Calibrate gamepad if needed

### Platform-Specific Issues

#### Windows
- Ensure XInput drivers are installed
- Check USB connection and power management settings
- Verify gamepad is recognized in Device Manager

#### Linux
- Check udev rules for gamepad access
- Verify evdev support
- Check permissions for /dev/input devices

#### macOS
- Verify MFi compliance for iOS controllers
- Check USB connection for wired controllers
- Verify Bluetooth pairing for wireless controllers 

## Performance Issues

### Low Framerate

| Symptom | Possible Causes | Solutions |
|---------|----------------|-----------|
| FPS below 60 | Too many draw calls | Use instanced rendering, merge meshes |
| | Complex shaders | Profile with RenderDoc, simplify shaders |
| | CPU-bound operations | Move work to background threads |
| | Memory leaks | Enable memory profiling, fix leaks |
| | GPU bottleneck | Reduce resolution, texture size, or effects |

#### Diagnosis Steps:

1. Enable the performance overlay: `engine.enablePerformanceOverlay(true);`
2. Check CPU vs GPU time to identify bottleneck
3. If CPU-bound:
   - Profile with Tracy: `engine.enableTracyProfiling(true);`
   - Look for hot functions consuming excessive CPU time
   - Check for excessive garbage collection or memory allocation
4. If GPU-bound:
   - Use RenderDoc to capture frames and analyze GPU workload
   - Check for shader complexity, excessive overdraw, or texture size issues
   - Monitor VRAM usage for potential texture streaming issues

#### Common Solutions:

- **Draw Call Optimization**:
  ```cpp
  // Bad: Many single mesh draws
  for (auto& mesh : meshes) {
      mesh.render(camera);
  }
  
  // Good: Instanced rendering
  auto instancedMesh = InstancedMesh("model.obj", meshes.size());
  for (size_t i = 0; i < meshes.size(); ++i) {
      instancedMesh.setTransform(i, meshes[i].getTransform());
  }
  instancedMesh.render(camera);
  ```

- **Shader Optimization**:
  ```glsl
  // Bad: Complex calculations in fragment shader
  void main() {
      vec3 result = vec3(0);
      for (int i = 0; i < 64; i++) {
          result += calculateLighting(i);
      }
      fragColor = vec4(result, 1.0);
  }
  
  // Good: Use light culling and simplified calculations
  void main() {
      vec3 result = vec3(0);
      for (int i = 0; i < lightCount; i++) {
          result += calculateSimplifiedLighting(i);
      }
      fragColor = vec4(result, 1.0);
  }
  ```

### Memory Leaks

| Symptom | Possible Causes | Solutions |
|---------|----------------|-----------|
| Increasing memory usage | Resource not released | Use smart pointers, implement proper cleanup |
| | Circular references | Break cycles with weak_ptr |
| | Cache not cleared | Implement size limits and TTL for caches |

#### Diagnosis:

1. Enable memory tracking: `engine.enableMemoryTracking(true);`
2. Check memory growth over time in the console output
3. Look for resource types showing continuous growth
4. Use memory profiling tools (Visual Studio Memory Profiler, Valgrind)

#### Solutions:

- Fix resource leaks with proper RAII:
  ```cpp
  // Bad: Manual resource management
  Texture* tex = new Texture("image.png");
  // ... code that might throw or return early ...
  delete tex; // Might never be reached
  
  // Good: RAII with smart pointers
  auto tex = std::make_shared<Texture>("image.png");
  // Resource automatically cleaned up when tex goes out of scope
  ```

- Fix circular references:
  ```cpp
  // Bad: Circular reference
  class Parent {
      std::shared_ptr<Child> child;
  };
  class Child {
      std::shared_ptr<Parent> parent; // Creates circular reference
  };
  
  // Good: Break cycle with weak_ptr
  class Parent {
      std::shared_ptr<Child> child;
  };
  class Child {
      std::weak_ptr<Parent> parent; // Doesn't prevent Parent from being destroyed
  };
  ```

### CPU Bottlenecks

| Symptom | Possible Causes | Solutions |
|---------|----------------|-----------|
| High CPU usage | Physics calculations | Use spatial partitioning, reduce simulation frequency |
| | AI pathfinding | Implement hierarchical pathfinding, update less frequently |
| | Complex animations | Reduce bone count, use LOD for distant models |
| | Scene traversal | Implement efficient culling strategies |

#### Solutions:

- Use spatial partitioning for physics:
  ```cpp
  // Create spatial grid for efficient collision detection
  SpatialGrid grid(100.0f, 100.0f, 5.0f); // 5.0f cell size
  
  // Add objects to grid
  for (auto& object : physicsObjects) {
      grid.addObject(object);
  }
  
  // Query only relevant objects for collision
  for (auto& object : physicsObjects) {
      auto nearbyObjects = grid.getNearbyObjects(object);
      for (auto& other : nearbyObjects) {
          checkCollision(object, other);
      }
  }
  ```

- Implement task-based multithreading:
  ```cpp
  // Distribute work across worker threads
  TaskScheduler scheduler(std::thread::hardware_concurrency());
  
  // Submit physics tasks in parallel
  auto physicsTask = scheduler.createTask([&]() {
      updatePhysics(deltaTime);
  });
  
  // Submit AI tasks in parallel
  auto aiTask = scheduler.createTask([&]() {
      updateAI(deltaTime);
  });
  
  // Wait for tasks to complete
  scheduler.waitForAll({physicsTask, aiTask});
  ```

## Rendering Problems

### Texture Issues

| Symptom | Possible Causes | Solutions |
|---------|----------------|-----------|
| Missing textures | File not found | Check file paths, use resource manager |
| | Incorrect format | Convert to supported format (PNG, JPG, TGA) |
| | Memory limits | Reduce texture size, implement streaming |
| Black textures | Incorrect UV coordinates | Check UVs in 3D modeling software |
| | Shader issue | Verify texture samplers are correct |

#### Diagnosis:

1. Enable verbose texture loading: `engine.setLogLevel(LogLevel::Verbose);`
2. Check console for texture loading errors
3. Verify file paths and formats
4. Use RenderDoc to inspect texture bindings and UV coordinates

### Shader Compilation Failures

| Symptom | Possible Causes | Solutions |
|---------|----------------|-----------|
| "Shader compilation failed" | Syntax error | Check shader syntax, fix errors |
| | Missing version directive | Add `#version 330 core` |
| | Unsupported features | Use only supported GLSL features |
| | Missing uniforms | Add all required uniforms |

#### Solutions:

- Check shader compilation log:
  ```cpp
  if (!shader.isCompiled()) {
      std::string log = shader.getCompilationLog();
      std::cout << "Shader compilation failed: " << log << std::endl;
  }
  ```

- Add proper version and feature directives:
  ```glsl
  #version 330 core
  #extension GL_ARB_explicit_uniform_location : enable
  
  layout(location = 0) uniform mat4 viewProjection;
  // ... rest of shader code ...
  ```

## Build System Issues

| Symptom | Possible Causes | Solutions |
|---------|----------------|-----------|
| CMake fails to find package | Package not installed | Run setup_dependencies script |
| | Incorrect path | Set proper CMAKE_PREFIX_PATH |
| | Version mismatch | Install required version |
| Linking errors | Missing library | Add library to target_link_libraries |
| | Dependency order | Correct link order in CMakeLists.txt |
| | Symbol visibility | Check export macros |

#### Solutions:

- Fix missing package:
  ```cmake
  # Set custom paths for finding packages
  set(CMAKE_PREFIX_PATH 
      ${CMAKE_PREFIX_PATH} 
      "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm"
      "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows")
  
  # Find package with specific version
  find_package(GLFW 3.3 REQUIRED)
  ```

- Fix linking errors:
  ```cmake
  # Ensure correct linking order
  target_link_libraries(SilentForgeEngine
      PRIVATE
      SilentForge::Core
      SilentForge::Rendering
      ${OPENGL_LIBRARIES}
      glfw
      ${GLFW_LIBRARIES}
  )
  ```

## Diagnostic Flowcharts

### Engine Startup Issues

```
Start
  |
  v
Is OpenGL context created? ---No---> Check GLFW initialization
  |                                    |
 Yes                                   v
  |                           GLFW init succeeded? ---No---> Check graphics drivers
  v                                    |
Shaders compile? ---No---> Check shader logs
  |                          |
 Yes                         v
  |                   Syntax error in shader? ---Yes---> Fix shader code
  v                          |
Models load? ---No---> Check model paths
  |                     |
 Yes                    v
  |              File exists? ---No---> Check working directory
  v                     |
Textures load? ---No--> Check texture paths
  |                      |
 Yes                     v
  |               Format supported? ---No---> Convert texture format
  v
Engine running correctly
```

### Performance Troubleshooting

```
Start
  |
  v
FPS below target? ---No---> No performance issue
  |
 Yes
  |
  v
Enable performance overlay
  |
  v
CPU time > GPU time? ---Yes---> CPU bottleneck
  |                               |
  No                              v
  |                      Run CPU profiler (Tracy)
  v                               |
GPU bottleneck                    v
  |                      Hot functions identified? ---Yes---> Optimize CPU code
  v                               |
Run GPU profiler (RenderDoc)      No
  |                               |
  v                               v
Many draw calls? ---Yes---> Use instanced rendering
  |                          Implement batching
  No                         Reduce object count
  |
  v
Complex shaders? ---Yes---> Simplify shader calculations
  |                          Use LOD for distant objects
  No
  |
  v
High resolution? ---Yes---> Reduce resolution
  |                          Implement dynamic resolution
  No
  |
  v
Large textures? ---Yes---> Reduce texture size
  |                         Implement texture compression
  No
  |
  v
Check for other GPU-intensive features:
- Post-processing
- Anti-aliasing
- Shadow quality
- Particle effects
```

### Memory Leak Diagnosis

```
Start
  |
  v
Memory usage increasing over time? ---No---> No memory leak
  |
 Yes
  |
  v
Enable memory tracking
  |
  v
Specific resource type growing? ---Yes---> Check resource management
  |                                          |
  No                                         v
  |                                 Using smart pointers? ---No---> Switch to smart pointers
  v                                          |
Check for circular references                Yes
  |                                          |
  v                                          v
Circular references found? ---Yes---> Break cycles with weak_ptr
  |                                   Review ownership patterns
  No
  |
  v
Check caching mechanisms
  |
  v
Unbounded cache growth? ---Yes---> Implement cache limits
  |                                 Add time-to-live (TTL)
  No
  |
  v
Run memory profiling tools:
- Visual Studio Memory Profiler
- Valgrind/Memcheck
- Address Sanitizer
``` 