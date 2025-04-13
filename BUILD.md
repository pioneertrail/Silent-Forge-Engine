# SilentForge Build Guide

## Prerequisites

### System Requirements
- CMake 3.16 or higher
- C++17 compatible compiler (GCC 8+, Clang 7+, MSVC 2019+)
- Git
- Make or Ninja

### Dependencies
- SDL2 (2.0.20 or higher)
- GLM (0.9.9 or higher)
- nlohmann_json (3.11.2 or higher)
- OpenGL (3.3 or higher)
- GLFW (3.3 or higher)
- GLAD

## Build Configuration

### CMake Options
| Option | Description | Default |
|--------|-------------|---------|
| `CMAKE_BUILD_TYPE` | Build type (Debug, Release, RelWithDebInfo) | Debug |
| `ENABLE_SANITIZERS` | Enable AddressSanitizer and ThreadSanitizer | OFF |
| `ENABLE_COVERAGE` | Enable test coverage reporting | OFF |
| `ENABLE_JSON_SUPPORT` | Enable JSON configuration support | ON |
| `ENABLE_RENDERING` | Enable rendering features | ON |

### Build Types
- **Debug**: Full debugging symbols, no optimizations, sanitizers enabled
- **Release**: Full optimizations, no debugging symbols
- **RelWithDebInfo**: Optimizations with debugging symbols

## Build Instructions

### Basic Build
```bash
# Clone the repository
git clone https://github.com/yourusername/SilentForge.git
cd SilentForge

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . --config Release
```

### Advanced Build Options
```bash
# Enable sanitizers and coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DENABLE_SANITIZERS=ON \
         -DENABLE_COVERAGE=ON

# Build with specific generator
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release

# Build with specific compiler
cmake .. -DCMAKE_C_COMPILER=gcc-10 -DCMAKE_CXX_COMPILER=g++-10
```

## Installation

### Default Installation
```bash
# Install to default locations
cmake --install .

# Install to custom prefix
cmake --install . --prefix /custom/install/path
```

### Installation Components
- **Runtime**: Executables and shared libraries
- **Development**: Headers and static libraries
- **Documentation**: API documentation and examples

## Testing

### Running Tests
```bash
# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R core_tests

# Run tests in parallel
ctest -j$(nproc)
```

### Coverage Reports
```bash
# Generate coverage report
cmake --build . --target coverage

# View coverage report
xdg-open coverage_report/index.html
```

## Development Workflow

### Debug Build
```bash
# Configure for debugging
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with debug symbols
cmake --build . --config Debug

# Run tests with sanitizers
ctest --output-on-failure
```

### Release Build
```bash
# Configure for release
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with optimizations
cmake --build . --config Release

# Run performance tests
ctest -R performance_tests
```

## IDE Integration

### VSCode
1. Install the CMake Tools extension
2. Open the project root and select a kit (e.g., GCC, Clang)
3. Configure with: `Ctrl+Shift+P` -> `CMake: Configure`
4. Build and run tests via the CMake Tools sidebar
5. Recommended extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - C++ TestMate (Mate Pek)
   - CodeLLDB (Vadim Chugunov)

### CLion
1. Open the project directory
2. CLion auto-detects the CMake configuration
3. Run tests via the Run Configurations menu
4. Recommended settings:
   - Enable Clang-Tidy
   - Enable Code Coverage
   - Enable Sanitizers for debug builds

### Visual Studio
1. Open the project directory
2. Select "Open a local folder"
3. Configure with CMake Settings
4. Build and run tests via the Test Explorer

## CI/CD Integration

### GitHub Actions
Example workflow for Linux builds:
```yaml
name: CI
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          build-essential \
          cmake \
          libsdl2-dev \
          libglm-dev \
          libjsoncpp-dev \
          libgl1-mesa-dev \
          libglfw3-dev
    
    - name: Configure
      run: |
        cmake -B build \
          -DCMAKE_BUILD_TYPE=Debug \
          -DENABLE_SANITIZERS=ON \
          -DENABLE_COVERAGE=ON
    
    - name: Build
      run: cmake --build build --parallel
    
    - name: Test
      run: cd build && ctest --output-on-failure
    
    - name: Upload coverage
      uses: codecov/codecov-action@v3
      with:
        file: build/coverage.info
```

### Azure DevOps
Example pipeline:
```yaml
trigger:
  - main

pool:
  vmImage: 'ubuntu-latest'

steps:
- task: CMake@1
  inputs:
    workingDirectory: '$(Build.SourcesDirectory)'
    cmakeArgs: '-B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON'

- task: CMake@1
  inputs:
    workingDirectory: '$(Build.SourcesDirectory)'
    cmakeArgs: '--build build --parallel'

- task: CMake@1
  inputs:
    workingDirectory: '$(Build.SourcesDirectory)'
    cmakeArgs: '--build build --target test'
```

## Troubleshooting

### Common Issues
1. **Missing Dependencies**
   - Install required packages using your system's package manager
   - Use `vcpkg` or `conan` for dependency management

2. **Compiler Errors**
   - Ensure C++17 support is enabled
   - Check compiler version compatibility

3. **Linker Errors**
   - Verify all dependencies are properly installed
   - Check library paths and rpath settings

### IDE-Specific Issues

#### VSCode
- **CMake Tools not detecting kit**: Try running `CMake: Scan for Kits`
- **IntelliSense errors**: Update `c_cpp_properties.json` with correct include paths
- **Test discovery failing**: Check `launch.json` configuration

#### CLion
- **CMake configuration errors**: Try invalidating caches (File -> Invalidate Caches)
- **Test runner issues**: Check Run/Debug Configurations
- **Sanitizer integration**: Ensure LLDB is properly configured

#### Visual Studio
- **CMake project not loading**: Check VS version compatibility
- **Test discovery**: Verify Test Adapter settings
- **Debug configuration**: Check launch.vs.json settings

### CI/CD Issues
- **Dependency installation**: Verify package names and versions
- **Build timeouts**: Adjust job timeouts in workflow
- **Test failures**: Check for missing OpenGL context in headless environments

### Getting Help
- Check the [GitHub Issues](https://github.com/yourusername/SilentForge/issues)
- Join the [Discord Community](https://discord.gg/your-server)
- Contact the maintainers at maintainers@silentforge.org

## Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests and ensure they pass
5. Submit a pull request

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines. 