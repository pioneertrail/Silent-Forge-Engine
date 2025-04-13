@echo off
SETLOCAL ENABLEEXTENSIONS

REM Set MinGW path
set PATH=C:\msys64\mingw64\bin;%PATH%

REM Check if vcpkg is initialized
if not exist vcpkg\vcpkg.exe (
    echo Bootstrapping vcpkg...
    cd vcpkg
    call bootstrap-vcpkg.bat
    cd ..
)

REM Install dependencies if needed
if not exist vcpkg\installed\x64-windows\include\nlohmann\json.hpp (
    echo Installing dependencies...
    vcpkg\vcpkg install nlohmann-json:x64-windows glm:x64-windows glfw3:x64-windows stb:x64-windows
)

REM Remove existing build directory
if exist build rmdir /s /q build
mkdir build
cd build

REM Run CMake to configure and build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows ..
cmake --build . --parallel
ctest -C Debug --verbose 