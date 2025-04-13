# SilentForge Engine Setup Guide

## Prerequisites

- **OS:** Windows 10+, macOS, or Linux
- **Compilers:** 
  - Windows: Visual Studio 2019+ or MinGW-64 (via [MSYS2](https://www.msys2.org/))
  - macOS: Xcode command line tools
  - Linux: GCC or Clang
- **Tools:** Git, CMake 3.14+

## Quick Setup

### Windows
```bash
# Clone repository with submodules
git clone --recursive https://github.com/yourusername/SilentForge.git
cd SilentForge

# Run setup script (MSVC)
scripts\setup_dependencies.bat

# For MinGW
scripts\setup_dependencies.bat mingw

# Build
build.bat  # For MSVC
build_mingw.bat  # For MinGW
```

### macOS/Linux
```bash
# Clone repository with submodules
git clone --recursive https://github.com/yourusername/SilentForge.git
cd SilentForge

# Run setup script
./scripts/setup_dependencies.sh

# Build
./build.sh
```

## Manual Setup

1. **Clone with submodules:**
   ```bash
   git clone --recursive https://github.com/yourusername/SilentForge.git
   cd SilentForge
   ```

2. **Configure with CMake:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

3. **Build:**
   ```bash
   cmake --build .
   ```

4. **Run tests:**
   ```bash
   ctest --output-on-failure
   ```

## Dependencies

The setup scripts automatically install all dependencies via vcpkg:
- GLM (Mathematics)
- GLFW (Window creation)
- Glad (OpenGL loader)
- STB (Image loading)
- nlohmann-json (JSON parsing)
- Catch2/Google Test (Testing)

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Missing dependencies | Run setup script again with admin privileges |
| Build errors | Check compiler version; delete build dir and retry |
| Path issues | Ensure tools are in PATH; restart terminal |

For detailed troubleshooting, see [Troubleshooting Guide](docs/TROUBLESHOOTING.md). 