@echo off
echo [MSVC Build] Setting up MSVC environment and building...

:: Set up Visual Studio environment (adjust path if needed)
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

:: Remove existing build directory
if exist build_msvc rmdir /s /q build_msvc
mkdir build_msvc
cd build_msvc

:: Configure with CMake using MSVC
echo [MSVC Build] Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DUSE_PRECOMPILED_HEADERS=ON ^
    -DCMAKE_TOOLCHAIN_FILE="..\vcpkg_config.cmake"

:: Build the project
echo [MSVC Build] Building the project...
cmake --build . --config Debug -- /m

:: Go back to the original directory
cd ..

echo [MSVC Build] Build completed! 