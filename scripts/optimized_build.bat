@echo off
setlocal enabledelayedexpansion

echo [OptimizedBuild] Starting optimized build for SilentForge...

:: Check if dependencies need to be preloaded
set SKIP_DEPENDENCIES=OFF
if "%1"=="--skip-deps" (
    set SKIP_DEPENDENCIES=ON
    echo [OptimizedBuild] Skipping dependency preloading...
) else (
    echo [OptimizedBuild] Preloading dependencies...
    call scripts\preload_dependencies.bat
)

:: Check if Ninja is installed
where ninja > nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [OptimizedBuild] Ninja not found. Installing via pip...
    pip install ninja
)

:: Check if ccache is installed
where ccache > nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [OptimizedBuild] ccache not found. Please install ccache from https://ccache.dev/
    echo [OptimizedBuild] Continuing without ccache...
) else (
    echo [OptimizedBuild] ccache found.
    set CMAKE_CXX_COMPILER_LAUNCHER=ccache
    set CMAKE_C_COMPILER_LAUNCHER=ccache
)

:: Create build directory
if exist build (
    echo [OptimizedBuild] Removing existing build directory...
    rmdir /s /q build
)
mkdir build
cd build

:: Run scripts to fix any build errors
echo [OptimizedBuild] Running fix_build_errors.py...
python ..\scripts\fix_build_errors.py

:: Configure with CMake using Ninja generator and optimizations
echo [OptimizedBuild] Configuring with CMake...
cmake -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_UNITY_BUILD=ON ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    -DUSE_PRECOMPILED_HEADERS=ON ^
    -DSKIP_DEPENDENCIES=%SKIP_DEPENDENCIES% ^
    -DCMAKE_TOOLCHAIN_FILE="..\vcpkg_config.cmake" ^
    ..

:: Build the project
echo [OptimizedBuild] Building the project...
cmake --build . --config Release -- -j %NUMBER_OF_PROCESSORS%

:: Run tests if requested
if "%1"=="--test" (
    echo [OptimizedBuild] Running tests...
    ctest --output-on-failure
)

echo [OptimizedBuild] Build complete!
cd ..

endlocal 