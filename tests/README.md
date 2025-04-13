# SilentForge Test Suite

## Overview
The test suite is divided into two main components:
- `core_tests`: Core functionality (SceneNode, Camera, Input, Config)
- `rendering_tests`: Graphics components (meshes, shaders, textures)

## Quick Start
```bash
# Configure with tests enabled
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build and run all tests
cmake --build .
ctest --output-on-failure

# Run specific test suites
./bin/tests/core_tests
./bin/tests/rendering_tests
```

## Test Organization

| Component | What's Tested |
|-----------|--------------|
| Core | SceneNode, Camera, InputManager, Gamepad, Config |
| Rendering | InstancedMesh, Shader, Texture |

## Adding Tests
1. Add test file to `CORE_TEST_SOURCES` or `RENDERING_TEST_SOURCES` in `tests/CMakeLists.txt`
2. Place fixtures in `tests/fixtures/`
3. Follow existing test patterns and naming conventions

## Test Fixtures
```
tests/fixtures/
  ├── shaders/  # Shader files for rendering tests
  ├── textures/ # Test textures
  └── models/   # 3D model files
```

## Debug Options
- `--gtest_break_on_failure`: Break into debugger on test failure
- `-DENABLE_SANITIZERS=ON`: Enable memory/thread sanitizers
- `-DENABLE_COVERAGE=ON`: Generate coverage reports

For comprehensive testing guidelines, see [Testing Guidelines](../docs/testing-guidelines.md). 