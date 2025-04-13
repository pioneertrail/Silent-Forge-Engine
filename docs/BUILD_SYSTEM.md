# SilentForge Build System Documentation

## Overview

The SilentForge build system has been optimized for faster and more reliable builds. This document outlines the build system architecture, optimization strategies, and usage instructions.

## Build System Components

### 1. Dependency Management (`scripts/preload_dependencies.ps1`)

The preload script manages all external dependencies using vcpkg:

```powershell
# Dependencies managed by vcpkg
- glm:x64-windows
- sdl2:x64-windows
- opengl:x64-windows
- gtest:x64-windows
- catch2:x64-windows
- nlohmann-json:x64-windows
- glfw3:x64-windows
```

**Benefits:**
- Consistent dependency versions
- Pre-downloaded dependencies
- No build-time downloads
- Cross-platform compatibility

### 2. Build Script (`scripts/build.ps1`)

The build script provides a unified interface for building the project:

```powershell
# Usage
./scripts/build.ps1 [-BuildType <Release|Debug>] [-Generator <GeneratorName>] [-Clean]

# Examples
./scripts/build.ps1 -BuildType Release -Clean
./scripts/build.ps1 -BuildType Debug -Generator "Visual Studio 17 2022"
```

**Features:**
- Automatic build directory management
- Parallel compilation support
- Clean build option
- Multiple generator support
- Error handling and reporting

### 3. CMake Configuration (`CMakeLists.txt`)

The CMake configuration has been optimized for performance:

#### Build Optimizations
```cmake
# Parallel compilation
if(MSVC)
    add_compile_options(/MP)  # MSVC parallel compilation
else()
    add_compile_options(-j)   # GCC/Clang parallel compilation
endif()

# Optimized compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3)
    endif()
endif()
```

#### Source Management
- Explicit source file lists instead of GLOB
- Proper include directory structure
- Optimized library linking

## Build Process

1. **Dependency Preloading**
   ```powershell
   ./scripts/preload_dependencies.ps1
   ```
   - Installs all required dependencies
   - Only needs to be run once or when dependencies change

2. **Project Building**
   ```powershell
   ./scripts/build.ps1 -BuildType Release -Clean
   ```
   - Configures CMake
   - Builds the project
   - Handles parallel compilation
   - Manages build artifacts

## Optimization Strategies

1. **Parallel Compilation**
   - MSVC: `/MP` flag
   - GCC/Clang: `-j` flag
   - Visual Studio: `--parallel` option

2. **Compiler Optimizations**
   - Release builds use maximum optimization
   - Platform-specific optimizations
   - Warning level management

3. **Dependency Management**
   - Preloaded dependencies
   - Consistent versions
   - No build-time downloads

4. **Build Directory Management**
   - Automatic creation
   - Clean build option
   - Proper artifact organization

## Best Practices

1. **Regular Builds**
   ```powershell
   ./scripts/build.ps1 -BuildType Release
   ```

2. **Clean Builds**
   ```powershell
   ./scripts/build.ps1 -BuildType Release -Clean
   ```

3. **Debug Builds**
   ```powershell
   ./scripts/build.ps1 -BuildType Debug
   ```

4. **Specific Generator**
   ```powershell
   ./scripts/build.ps1 -Generator "Visual Studio 17 2022"
   ```

## Troubleshooting

1. **Build Failures**
   - Check dependency installation
   - Verify compiler settings
   - Ensure proper generator selection

2. **Performance Issues**
   - Use clean builds when necessary
   - Verify parallel compilation
   - Check system resources

3. **Dependency Issues**
   - Run preload script
   - Verify vcpkg installation
   - Check network connectivity

## Future Improvements

1. **Incremental Builds**
   - Optimize dependency tracking
   - Improve rebuild detection

2. **Cache Management**
   - Implement build cache
   - Optimize artifact storage

3. **Cross-Platform Support**
   - Enhance platform detection
   - Improve generator selection

4. **Dependency Updates**
   - Automated version checking
   - Dependency update notifications 