@echo off
setlocal enabledelayedexpansion

echo === Simple Build Benchmark ===
echo This script will test basic build optimizations without dependencies

set CMAKE_BASE=-DSKIP_DEPENDENCIES=ON -DBUILD_TESTING=OFF

:: Create the build directories
set BUILD_NORMAL=build_normal
set BUILD_FAST=build_fast

:: Ensure we're using absolute paths
set VCPKG_PATH=%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake

:: Clean and prepare
if exist %BUILD_NORMAL% rmdir /s /q %BUILD_NORMAL%
if exist %BUILD_FAST% rmdir /s /q %BUILD_FAST%
mkdir %BUILD_NORMAL%
mkdir %BUILD_FAST%

:: Standard build (no optimizations)
echo.
echo === Standard build (no optimizations) ===
cd %BUILD_NORMAL%
set start_time=%time%
cmake .. -G "MinGW Makefiles" %CMAKE_BASE%
if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed!
    cd ..
    goto :error
)
cmake --build .
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" normal_time
echo Standard build completed in %normal_time% seconds
cd ..

:: Fast build (with optimizations)
echo.
echo === Fast build (Ninja + ccache + PCH) ===
cd %BUILD_FAST%
set start_time=%time%
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DUSE_PRECOMPILED_HEADERS=ON %CMAKE_BASE%
if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed!
    cd ..
    goto :error
)
cmake --build . -j %NUMBER_OF_PROCESSORS%
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" fast_time
echo Fast build completed in %fast_time% seconds
cd ..

:: Calculate speedup
if defined normal_time (
    if defined fast_time (
        set /a speedup=(!normal_time! * 100) / !fast_time!
        for /f "tokens=1,2 delims=." %%a in ("!speedup!.00") do (
            set speedup_str=%%a.%%b
        )
        echo.
        echo === Benchmark Results ===
        echo Standard build: %normal_time% seconds
        echo Fast build: %fast_time% seconds
        echo Speedup: %speedup_str%x
    )
)

exit /b 0

:error
echo An error occurred during benchmarking.
exit /b 1

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