@echo off
setlocal EnableDelayedExpansion

echo === SilentForge Developer Build ===
echo This script configures and builds the project with optimized settings for faster development cycles.

REM Check if Python is available
where python >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Python is required but not found in PATH
    exit /b 1
)

REM Run dependency guard script to prepare the build
echo Running dependency guard...
python scripts\dependency_guard.py
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to run dependency guard script
    exit /b 1
)

REM Check for ccache
set USE_CCACHE=OFF
where ccache >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo Found ccache - will use for faster incremental builds
    set USE_CCACHE=ON
)

REM Check for Ninja build system
set GENERATOR="Visual Studio 17 2022"
set BUILD_TOOL=msbuild
where ninja >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo Found Ninja - will use for faster parallel builds
    set GENERATOR="Ninja"
    set BUILD_TOOL=ninja
)

REM Create build directory if it doesn't exist
if not exist build_dev mkdir build_dev
cd build_dev

REM Record start time
set START_TIME=%TIME%

REM Configure the project with CMake
echo Configuring with CMake...
cmake .. -G %GENERATOR% ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DSKIP_DEPENDENCIES=ON ^
    -DUSE_PRECOMPILED_HEADERS=ON ^
    -DCMAKE_C_COMPILER_LAUNCHER=ccache ^
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed
    cd ..
    exit /b 1
)

REM Build the project
echo Building SilentForge...
cmake --build . --config Debug

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed
    cd ..
    exit /b 1
)

REM Calculate elapsed time
set END_TIME=%TIME%

REM Convert time to centiseconds for easier calculation
for /F "tokens=1-4 delims=:.," %%a in ("%START_TIME%") do (
    set /A "START_TOTAL=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
for /F "tokens=1-4 delims=:.," %%a in ("%END_TIME%") do (
    set /A "END_TOTAL=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

REM Calculate elapsed time in centiseconds
set /A ELAPSED_TOTAL=END_TOTAL-START_TOTAL
if %ELAPSED_TOTAL% LSS 0 set /A ELAPSED_TOTAL+=24*60*60*100

REM Convert to hours, minutes, seconds and centiseconds
set /A ELAPSED_HOURS=ELAPSED_TOTAL/(60*60*100)
set /A ELAPSED_MINUTES=(ELAPSED_TOTAL/(60*100))%%60
set /A ELAPSED_SECONDS=(ELAPSED_TOTAL/100)%%60
set /A ELAPSED_CENTISECONDS=ELAPSED_TOTAL%%100

echo Build completed in %ELAPSED_HOURS%h %ELAPSED_MINUTES%m %ELAPSED_SECONDS%.%ELAPSED_CENTISECONDS%s

echo.
echo ===================================================
echo To rebuild after changes, simply run scripts\dev_build.bat again
echo ===================================================

REM Return to original directory
cd ..

endlocal 