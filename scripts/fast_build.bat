@echo off
setlocal enabledelayedexpansion

echo [FastBuild] Starting optimized build process...

REM Check for Python
where python >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [FastBuild] ERROR: Python is not installed or not in PATH
    exit /b 1
)

REM Check for Ninja
where ninja >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [FastBuild] WARNING: Ninja build system not found, will use default generator
    set USE_NINJA=0
) else (
    echo [FastBuild] Ninja build system found
    set USE_NINJA=1
)

REM Check for ccache
where ccache >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [FastBuild] WARNING: ccache not found, build will not be accelerated
    set USE_CCACHE=0
) else (
    echo [FastBuild] ccache found
    set USE_CCACHE=1
)

REM Run script to fix build errors
echo [FastBuild] Running build error fix script...
python scripts/fix_build_errors.py

REM Create build directory if it doesn't exist
echo [FastBuild] Setting up build directory...
if not exist build mkdir build
cd build

REM Configure CMake options
set CMAKE_OPTIONS=-DCMAKE_BUILD_TYPE=Debug -DSKIP_DEPENDENCIES=ON

REM Add Ninja generator if available
if %USE_NINJA% equ 1 (
    set CMAKE_OPTIONS=!CMAKE_OPTIONS! -G Ninja
)

REM Add ccache if available
if %USE_CCACHE% equ 1 (
    set CMAKE_OPTIONS=!CMAKE_OPTIONS! -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
)

REM Add precompiled headers option
set CMAKE_OPTIONS=!CMAKE_OPTIONS! -DUSE_PRECOMPILED_HEADERS=ON

REM Configure and build
echo [FastBuild] Configuring project with options: !CMAKE_OPTIONS!
cmake .. !CMAKE_OPTIONS!

REM Record start time
set START_TIME=%TIME%

echo [FastBuild] Building project...
cmake --build . --config Debug -j %NUMBER_OF_PROCESSORS%

REM Calculate build time
set END_TIME=%TIME%

REM Convert time to seconds for comparison
for /F "tokens=1-4 delims=:.," %%a in ("%START_TIME%") do (
   set /A "start=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
for /F "tokens=1-4 delims=:.," %%a in ("%END_TIME%") do (
   set /A "end=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
set /A elapsed=end-start
set /A hh=elapsed/(60*60*100), rest=elapsed%%(60*60*100), mm=rest/(60*100), rest%%=60*100, ss=rest/100, cc=rest%%100
if %hh% lss 10 set hh=0%hh%
if %mm% lss 10 set mm=0%mm%
if %ss% lss 10 set ss=0%ss%
if %cc% lss 10 set cc=0%cc%

echo [FastBuild] Build completed in %hh%:%mm%:%ss%.%cc%

REM Return to original directory
cd ..

echo [FastBuild] Build process finished. 