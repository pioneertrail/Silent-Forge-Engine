@echo off
setlocal enabledelayedexpansion

echo === Build Optimization Benchmark ===

:: Set up common parameters
set VCPKG_TOOLCHAIN=-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

:: Benchmark vanilla build (no optimizations)
echo.
echo === Testing baseline build (no optimizations) ===
call :run_benchmark vanilla "-G MinGW Makefiles"

:: Benchmark with Ninja only
echo.
echo === Testing with Ninja only ===
call :run_benchmark ninja "-G Ninja"

:: Benchmark with ccache only
echo.
echo === Testing with ccache only ===
call :run_benchmark ccache "-G MinGW Makefiles -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"

:: Benchmark with precompiled headers only
echo.
echo === Testing with precompiled headers only ===
call :run_benchmark pch "-G MinGW Makefiles -DUSE_PRECOMPILED_HEADERS=ON"

:: Benchmark with Ninja + ccache
echo.
echo === Testing with Ninja + ccache ===
call :run_benchmark ninja_ccache "-G Ninja -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"

:: Benchmark with all optimizations
echo.
echo === Testing with all optimizations ===
call :run_benchmark full_opt "-G Ninja -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DUSE_PRECOMPILED_HEADERS=ON"

:: Print summary
echo.
echo === Build Optimization Summary ===
echo Baseline (no optimizations): !vanilla_time! seconds
echo Ninja only: !ninja_time! seconds (speedup: !ninja_speedup!x)
echo ccache only: !ccache_time! seconds (speedup: !ccache_speedup!x)
echo PCH only: !pch_time! seconds (speedup: !pch_speedup!x)
echo Ninja + ccache: !ninja_ccache_time! seconds (speedup: !ninja_ccache_speedup!x)
echo All optimizations: !full_opt_time! seconds (speedup: !full_opt_speedup!x)

exit /b 0

:run_benchmark
set build_name=%~1
set build_flags=%~2
set build_dir=build_%build_name%

echo Building in %build_dir% with flags: %build_flags%

:: Clean and prepare
if exist %build_dir% rmdir /s /q %build_dir%
mkdir %build_dir%
pushd %build_dir%

:: First build - cold cache
echo Configuring...
set start_time=%time%
cmake .. %VCPKG_TOOLCHAIN% %build_flags%
if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed!
    popd
    exit /b 1
)

echo Building (first time)...
cmake --build . -j %NUMBER_OF_PROCESSORS%
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    popd
    exit /b 1
)
set end_time=%time%

:: Calculate cold build time
call :calc_time_diff "%start_time%" "%end_time%" cold_time
echo Cold build completed in %cold_time% seconds

:: Touch a source file to force incremental build
pushd ..
echo Touching source file...
if exist src\core\Application.cpp (
    copy src\core\Application.cpp src\core\Application.cpp.bak > nul
    copy src\core\Application.cpp +,, > nul
) else if exist src\app\main.cpp (
    copy src\app\main.cpp src\app\main.cpp.bak > nul
    copy src\app\main.cpp +,, > nul
)
popd

:: Second build - incremental
echo Building (incremental)...
set start_time=%time%
cmake --build . -j %NUMBER_OF_PROCESSORS%
set end_time=%time%

:: Calculate incremental build time
call :calc_time_diff "%start_time%" "%end_time%" incr_time
echo Incremental build completed in %incr_time% seconds

:: Restore touched files
pushd ..
if exist src\core\Application.cpp.bak (
    move /Y src\core\Application.cpp.bak src\core\Application.cpp > nul
)
if exist src\app\main.cpp.bak (
    move /Y src\app\main.cpp.bak src\app\main.cpp > nul
)
popd

:: Save results
set /a %build_name%_time=%cold_time%
set /a %build_name%_incr=%incr_time%

:: Calculate speedup vs vanilla (if we have vanilla results)
if defined vanilla_time (
    set /a %build_name%_speedup_int=(!vanilla_time! * 100) / !%build_name%_time!
    for /f "tokens=1,2 delims=." %%a in ("!%build_name%_speedup_int!.00") do (
        set %build_name%_speedup=%%a.%%b
    )
)

popd
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