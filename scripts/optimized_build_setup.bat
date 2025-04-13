@echo off
setlocal enabledelayedexpansion

echo ===================================================
echo SilentForge Optimized Build Setup
echo ===================================================
echo.

REM Set start time for overall script
set start_time=%time%

REM Create scripts directory if it doesn't exist
if not exist "scripts" mkdir scripts

REM Check if Python is installed
python --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Python is not installed or not in PATH.
    echo Please install Python 3.6 or newer.
    exit /b 1
)

REM Initialize vcpkg if it doesn't exist
if not exist "vcpkg" (
    echo Cloning vcpkg repository...
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    call bootstrap-vcpkg.bat
    cd ..
) else (
    echo vcpkg directory already exists, skipping clone.
)

REM Make sure vcpkg is bootstrapped
if not exist vcpkg\vcpkg.exe (
    echo Bootstrapping vcpkg...
    cd vcpkg
    call bootstrap-vcpkg.bat
    cd ..
)

REM Add vcpkg to PATH
set PATH=%CD%\vcpkg;%PATH%

REM Check if ccache is installed
where ccache >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Installing ccache via vcpkg...
    vcpkg install ccache:x64-windows
    set PATH=%CD%\vcpkg\installed\x64-windows\tools\ccache;%PATH%
)

REM Check for Ninja
where ninja >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Installing Ninja via vcpkg...
    vcpkg install ninja:x64-windows
    set PATH=%CD%\vcpkg\installed\x64-windows\tools;%PATH%
)

echo.
echo Setting up vcpkg dependencies...
echo.

set dep_start_time=%time%

REM Install dependencies
echo Installing dependencies with vcpkg...
vcpkg install sdl2:x64-windows glew:x64-windows glm:x64-windows assimp:x64-windows bullet3:x64-windows openal-soft:x64-windows

REM Calculate dependency installation time
set dep_end_time=%time%
call :timediff dep_elapsed_time "%dep_start_time%" "%dep_end_time%"
echo Dependency installation completed in %dep_elapsed_time%

REM Run the dependency guard script to fix CMakeLists.txt
echo Fixing CMakeLists.txt for optimized builds...
python scripts\dependency_guard.py

REM Create optimized build directory
echo Setting up optimized build with Ninja and ccache...
if exist "build_optimized" (
    echo Removing existing build_optimized directory...
    rmdir /s /q build_optimized
)

mkdir build_optimized
cd build_optimized

REM Set environment for ccache (if installed)
set "PATH=!PATH!;%cd%\..\vcpkg\installed\x64-windows\tools\ccache"
set CMAKE_C_COMPILER_LAUNCHER=ccache
set CMAKE_CXX_COMPILER_LAUNCHER=ccache

REM Get ccache stats before build
echo Initial ccache stats:
ccache -s

REM Setup the build with CMake
echo Configuring project with CMake...
set build_start_time=%time%

cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ^
         -DCMAKE_BUILD_TYPE=Release ^
         -G "Ninja" ^
         -DUSE_PRECOMPILED_HEADERS=ON ^
         -DSKIP_DEPENDENCIES=OFF

echo Building project...
cmake --build . --config Release -- -j %NUMBER_OF_PROCESSORS%

REM Get ccache stats after build
echo Final ccache stats:
ccache -s

REM Calculate build time
set build_end_time=%time%
call :timediff build_elapsed_time "%build_start_time%" "%build_end_time%"
echo.
echo Project built in %build_elapsed_time%

REM Create a development setup for faster incremental builds
echo.
echo Setting up fast development build configuration...
if exist "build_dev" (
    echo Removing existing build_dev directory...
    rmdir /s /q build_dev
)

mkdir build_dev
cd build_dev

echo Configuring fast development build...
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ^
         -DCMAKE_BUILD_TYPE=Debug ^
         -G "Ninja" ^
         -DUSE_PRECOMPILED_HEADERS=ON ^
         -DSKIP_DEPENDENCIES=ON

echo.
echo Fast development build configured. For incremental builds:
echo cd build_dev
echo cmake --build . -- -j %NUMBER_OF_PROCESSORS%
echo.

cd ..

REM Calculate total script time
set end_time=%time%
call :timediff total_elapsed_time "%start_time%" "%end_time%"

echo ===================================================
echo Build Setup Complete!
echo ===================================================
echo Optimized build created in: %cd%\build_optimized
echo Development build created in: %cd%\build_dev
echo Total setup time: %total_elapsed_time%
echo.
echo For incremental builds, use:
echo   - Full build: cmake --build build_optimized -- -j %NUMBER_OF_PROCESSORS%
echo   - Dev build: cmake --build build_dev -- -j %NUMBER_OF_PROCESSORS%
echo ===================================================

goto :eof

:timediff
setlocal
set start=%~2
set end=%~3

REM Convert start time to seconds
for /F "tokens=1-4 delims=:,. " %%a in ("%start%") do (
   set /A "start_s=(((%%a*60)+1%%b%%100)*60+1%%c%%100)*100+1%%d%%100-100"
)

REM Convert end time to seconds
for /F "tokens=1-4 delims=:,. " %%a in ("%end%") do (
   set /A "end_s=(((%%a*60)+1%%b%%100)*60+1%%c%%100)*100+1%%d%%100-100"
)

REM Calculate the elapsed time in seconds
set /A elapsed=end_s-start_s
if %elapsed% lss 0 set /A elapsed+=24*60*60*100

REM Convert back to hh:mm:ss.mm format
set /A "hh=elapsed/(60*60*100), rest=elapsed%%(60*60*100), mm=rest/(60*100), rest%%=60*100, ss=rest/100, cc=rest%%100"

REM Return the result
endlocal & set %1=%elapsed:~0,-2%.%elapsed:~-2%
goto :eof 