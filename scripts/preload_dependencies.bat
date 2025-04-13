@echo off
setlocal enabledelayedexpansion

echo [PreloadDependencies] Setting up dependencies for SilentForge...

:: Check for vcpkg directory
if not exist vcpkg (
    echo [PreloadDependencies] vcpkg not found. Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg.git
    pushd vcpkg
    call bootstrap-vcpkg.bat
    popd
) else (
    echo [PreloadDependencies] vcpkg found. Updating...
    pushd vcpkg
    git pull
    popd
)

:: Set environment variables
set VCPKG_ROOT=%cd%\vcpkg
set PATH=%VCPKG_ROOT%;%PATH%

:: Install dependencies
echo [PreloadDependencies] Installing dependencies...
pushd vcpkg
vcpkg install sdl2:x64-windows
vcpkg install glew:x64-windows
vcpkg install glm:x64-windows
vcpkg install imgui:x64-windows
vcpkg install stb:x64-windows
vcpkg install nlohmann-json:x64-windows
vcpkg install spdlog:x64-windows
vcpkg install assimp:x64-windows
vcpkg integrate install
popd

:: Add CMake toolchain file configuration to a local file
echo [PreloadDependencies] Configuring CMake toolchain file...
echo set(CMAKE_TOOLCHAIN_FILE "%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" CACHE STRING "") > vcpkg_config.cmake
echo set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "") >> vcpkg_config.cmake

echo [PreloadDependencies] Dependencies successfully installed and configured.
echo [PreloadDependencies] Use -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" when running CMake.
echo [PreloadDependencies] Or include vcpkg_config.cmake in your CMake command.

endlocal 