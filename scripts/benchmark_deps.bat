@echo off
setlocal enabledelayedexpansion

echo === Dependency Benchmark Script ===

set BUILD_DIR=build_bench
set VCPKG_TOOLCHAIN=-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
set CMAKE_OPTS=-G Ninja -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

:: Store current directory
set CURRENT_DIR=%CD%

:: Clean start
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
mkdir %BUILD_DIR%
cd %BUILD_DIR%

:: Run a series of benchmarks, adding one dependency at a time
echo Running baseline build (no dependencies)...
call :benchmark_dependency "BASE" "-DSKIP_DEPENDENCIES=ON"

echo Adding OpenGL...
call :benchmark_dependency "OPENGL" "-DUSE_OPENGL=ON"

echo Adding GLM...
call :benchmark_dependency "GLM" "-DUSE_OPENGL=ON -DUSE_GLM=ON"

echo Adding SDL2...
call :benchmark_dependency "SDL2" "-DUSE_OPENGL=ON -DUSE_GLM=ON -DUSE_SDL2=ON"

echo Adding JSON...
call :benchmark_dependency "JSON" "-DUSE_OPENGL=ON -DUSE_GLM=ON -DUSE_SDL2=ON -DUSE_JSON=ON"

echo Adding GLFW...
call :benchmark_dependency "GLFW" "-DUSE_OPENGL=ON -DUSE_GLM=ON -DUSE_SDL2=ON -DUSE_JSON=ON -DUSE_GLFW=ON"

echo Running full build with all dependencies...
call :benchmark_dependency "FULL" "%VCPKG_TOOLCHAIN%"

:: Print results
echo.
echo === Dependency Benchmark Results ===
echo Baseline (no deps): !BASE_time! seconds
echo +OpenGL: !OPENGL_time! seconds (!OPENGL_diff! seconds added)
echo +GLM: !GLM_time! seconds (!GLM_diff! seconds added)
echo +SDL2: !SDL2_time! seconds (!SDL2_diff! seconds added)
echo +JSON: !JSON_time! seconds (!JSON_diff! seconds added)
echo +GLFW: !GLFW_time! seconds (!GLFW_diff! seconds added)
echo Full build: !FULL_time! seconds (!FULL_diff! seconds added)

cd %CURRENT_DIR%
exit /b 0

:benchmark_dependency
setlocal
set name=%~1
set deps=%~2
set prev_name=!last_name!

:: Always make sure we're in the build directory
cd %CURRENT_DIR%\%BUILD_DIR%

:: Configure with the requested dependencies
echo === Configuring with %name% dependencies ===
set start_time=%time%
cmake .. %CMAKE_OPTS% %deps%
if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed!
    endlocal
    exit /b 1
)

:: Build
echo === Building with %name% dependencies ===
cmake --build . -j %NUMBER_OF_PROCESSORS% --clean-first
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    endlocal
    exit /b 1
)

:: Calculate and store time
set end_time=%time%
call :calc_time_diff "%start_time%" "%end_time%" time_diff
set last_name=%name%

:: Save the results to parent scope
endlocal & set %name%_time=%time_diff% & set last_name=%name%

:: Calculate diff from previous if not the first run
if not "%prev_name%"=="" (
    set /a %name%_diff=!%name%_time! - !%prev_name%_time!
)

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