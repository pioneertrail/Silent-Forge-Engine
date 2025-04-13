@echo off
setlocal enabledelayedexpansion

echo === SilentForge Dependency Setup ===
echo This script will setup all required dependencies using vcpkg

:: Initialize vcpkg if needed
if not exist vcpkg\vcpkg.exe (
    echo Step 1: Initializing vcpkg...
    if not exist vcpkg (
        git clone https://github.com/Microsoft/vcpkg.git
    )
    cd vcpkg
    call bootstrap-vcpkg.bat
    cd ..
) else (
    echo Step 1: vcpkg already initialized.
)

:: Install dependencies
echo.
echo Step 2: Installing dependencies...

set start_time=%time%

set VCPKG_DEFAULT_TRIPLET=x64-windows
cd vcpkg
echo Installing SDL2...
call vcpkg install sdl2
echo Installing GLEW...
call vcpkg install glew
echo Installing GLM...
call vcpkg install glm
echo Installing Assimp...
call vcpkg install assimp
echo Installing OpenGL...
call vcpkg install opengl
echo Installing Bullet...
call vcpkg install bullet3
echo Installing OpenAL...
call vcpkg install openal-soft
cd ..

set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" dep_time
echo.
echo === Dependencies installed in %dep_time% seconds ===

:: Build with dependencies
echo.
echo Step 3: Setting up build with dependencies
set BUILD_DIR=build_full
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
mkdir %BUILD_DIR%

cd %BUILD_DIR%
set start_time=%time%

echo Running CMake configuration with dependencies...
cmake .. -G "Ninja" ^
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ^
    -DUSE_PRECOMPILED_HEADERS=ON ^
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed!
    cd ..
    exit /b 1
)

echo Building full project with dependencies...
cmake --build . -j %NUMBER_OF_PROCESSORS%
set end_time=%time%

cd ..
call :calc_time_diff "%start_time%" "%end_time%" build_time
echo.
echo === Full build completed in %build_time% seconds ===
echo.
echo === Build Summary ===
echo The full build with dependencies has been created in: %BUILD_DIR%
echo For future full builds, use:
echo   cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DUSE_PRECOMPILED_HEADERS=ON

exit /b 0

:calc_time_diff
setlocal
set start=%~1
set end=%~2

:: Convert start time to seconds
set /a start_h=1%start:~0,2%-100
set /a start_m=1%start:~3,2%-100
set /a start_s=1%start:~6,2%-100
set /a start_total=%start_h%*3600 + %start_m%*60 + %start_s%

:: Convert end time to seconds
set /a end_h=1%end:~0,2%-100
set /a end_m=1%end:~3,2%-100
set /a end_s=1%end:~6,2%-100
set /a end_total=%end_h%*3600 + %end_m%*60 + %end_s%

:: Calculate difference
set /a diff=%end_total% - %start_total%
if %diff% lss 0 set /a diff+=86400

endlocal & set %~3=%diff%
exit /b 0 