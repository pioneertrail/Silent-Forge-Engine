@echo off
echo ===================================================
echo  SilentForge Build Benchmark Suite
echo ===================================================
echo.
echo This script will guide you through benchmarking your build system.
echo.

:menu
echo What would you like to benchmark?
echo.
echo 1. Run optimization benchmarks (Ninja, ccache, PCH)
echo 2. Run dependency benchmarks (OpenGL, GLM, SDL2, etc.)
echo 3. Patch CMakeLists.txt for benchmarking
echo 4. Restore original CMakeLists.txt
echo 5. Exit
echo.

set /p choice=Enter your choice (1-5): 

if "%choice%"=="1" goto :opt_benchmark
if "%choice%"=="2" goto :dep_benchmark
if "%choice%"=="3" goto :patch_cmakelists
if "%choice%"=="4" goto :restore_cmakelists
if "%choice%"=="5" goto :eof

echo Invalid choice. Please try again.
goto :menu

:opt_benchmark
cls
echo ===================================================
echo  Optimization Benchmark
echo ===================================================
echo.
echo This will benchmark different build optimizations:
echo  - Ninja build system
echo  - ccache compiler cache
echo  - Precompiled headers
echo  - Combinations of the above
echo.
echo The benchmark will take some time to run.
echo.
set /p confirm=Start optimization benchmark? (y/n): 

if /i "%confirm%"=="y" (
    call scripts\benchmark_opts.bat
    echo.
    echo Benchmark complete! Press any key to return to the menu...
    pause > nul
) else (
    echo Benchmark cancelled.
)
goto :menu

:dep_benchmark
cls
echo ===================================================
echo  Dependency Benchmark
echo ===================================================
echo.
echo This will benchmark the impact of each dependency on build time:
echo  - OpenGL
echo  - GLM
echo  - SDL2
echo  - JSON
echo  - GLFW
echo.
echo The benchmark will take some time to run.
echo.
set /p confirm=Start dependency benchmark? (y/n): 

if /i "%confirm%"=="y" (
    call scripts\benchmark_deps.bat
    echo.
    echo Benchmark complete! Press any key to return to the menu...
    pause > nul
) else (
    echo Benchmark cancelled.
)
goto :menu

:patch_cmakelists
cls
echo ===================================================
echo  Patch CMakeLists.txt
echo ===================================================
echo.
echo This will modify your CMakeLists.txt to support benchmarking.
echo A backup will be created as CMakeLists.txt.bak.
echo.
set /p confirm=Patch CMakeLists.txt? (y/n): 

if /i "%confirm%"=="y" (
    call scripts\patch_cmakelists.bat
    echo.
    echo Patching complete! Press any key to return to the menu...
    pause > nul
) else (
    echo Patching cancelled.
)
goto :menu

:restore_cmakelists
cls
echo ===================================================
echo  Restore CMakeLists.txt
echo ===================================================
echo.
echo This will restore your original CMakeLists.txt from the backup.
echo.
if not exist CMakeLists.txt.bak (
    echo No backup found! Cannot restore.
    echo Press any key to return to the menu...
    pause > nul
    goto :menu
)

set /p confirm=Restore original CMakeLists.txt? (y/n): 

if /i "%confirm%"=="y" (
    copy CMakeLists.txt.bak CMakeLists.txt
    echo Original CMakeLists.txt restored!
    echo Press any key to return to the menu...
    pause > nul
) else (
    echo Restore cancelled.
)
goto :menu 