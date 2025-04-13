@echo off
setlocal enabledelayedexpansion

echo === Build Benchmark Script ===

:: Parse arguments
set USE_NINJA=0
set USE_CCACHE=0
set USE_PCH=0
set BUILD_DIR=build_benchmark
set CLEAN=1

:parse_args
if "%1"=="" goto :done_parsing
if /i "%1"=="--ninja" set USE_NINJA=1
if /i "%1"=="--ccache" set USE_CCACHE=1
if /i "%1"=="--pch" set USE_PCH=1
if /i "%1"=="--no-clean" set CLEAN=0
if /i "%1"=="--dir" set BUILD_DIR=%2 & shift
shift
goto :parse_args
:done_parsing

:: Setup environment
set PATH=%PATH%;C:\msys64\mingw64\bin;C:\Strawberry\c\bin

:: Report configuration
echo Build configuration:
echo   Build directory: %BUILD_DIR%
echo   Using Ninja: %USE_NINJA%
echo   Using ccache: %USE_CCACHE%
echo   Using PCH: %USE_PCH%
echo   Clean build: %CLEAN%

:: Setup clean build if requested
if %CLEAN%==1 (
    echo Cleaning build directory...
    if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

:: Prepare cmake arguments
set CMAKE_ARGS=-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_TESTING=ON

:: Add ninja if selected
if %USE_NINJA%==1 (
    set CMAKE_ARGS=!CMAKE_ARGS! -G "Ninja"
    echo Using Ninja build system
)

:: Add ccache if selected
if %USE_CCACHE%==1 (
    set CMAKE_ARGS=!CMAKE_ARGS! -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
    echo Using ccache
)

:: Add PCH flag if selected (will be handled in the adjusted CMakeLists.txt)
if %USE_PCH%==1 (
    set CMAKE_ARGS=!CMAKE_ARGS! -DUSE_PRECOMPILED_HEADERS=ON
    echo Using precompiled headers
)

:: Configure step
echo === Starting configuration ===
set config_start=%time%
cmake .. %CMAKE_ARGS%
set config_status=%ERRORLEVEL%
set config_end=%time%

:: Calculate configuration time
call :time_diff "%config_start%" "%config_end%" config_time
echo Configuration completed in %config_time% seconds with status %config_status%

:: If configuration failed, exit
if %config_status% NEQ 0 (
    echo Configuration failed!
    cd ..
    exit /b 1
)

:: First build (cold)
echo === First Build (Cold Cache) ===
set build1_start=%time%
cmake --build . -j %NUMBER_OF_PROCESSORS%
set build1_status=%ERRORLEVEL%
set build1_end=%time%

:: Calculate first build time
call :time_diff "%build1_start%" "%build1_end%" build1_time
echo First build completed in %build1_time% seconds with status %build1_status%

:: If build failed, exit
if %build1_status% NEQ 0 (
    echo Build failed!
    cd ..
    exit /b 1
)

:: Touch a source file to force partial rebuild
echo Touching a source file for rebuild test...
if exist ..\src\core\Application.cpp (
    copy ..\src\core\Application.cpp ..\src\core\Application.cpp.bak
    copy ..\src\core\Application.cpp +,,
    echo Modified Application.cpp
) else if exist ..\src\app\main.cpp (
    copy ..\src\app\main.cpp ..\src\app\main.cpp.bak
    copy ..\src\app\main.cpp +,,
    echo Modified main.cpp
)

:: Second build (incremental)
echo === Second Build (Incremental) ===
set build2_start=%time%
cmake --build . -j %NUMBER_OF_PROCESSORS%
set build2_status=%ERRORLEVEL%
set build2_end=%time%

:: Calculate second build time
call :time_diff "%build2_start%" "%build2_end%" build2_time
echo Second build completed in %build2_time% seconds with status %build2_status%

:: Restore modified files
if exist ..\src\core\Application.cpp.bak (
    move /Y ..\src\core\Application.cpp.bak ..\src\core\Application.cpp > nul
)
if exist ..\src\app\main.cpp.bak (
    move /Y ..\src\app\main.cpp.bak ..\src\app\main.cpp > nul
)

:: Run tests
echo === Running Tests ===
set test_start=%time%
ctest --output-on-failure
set test_status=%ERRORLEVEL%
set test_end=%time%

:: Calculate test time
call :time_diff "%test_start%" "%test_end%" test_time
echo Tests completed in %test_time% seconds with status %test_status%

:: Summary
echo === Build Benchmark Summary ===
echo Configuration time: %config_time% seconds
echo First build time: %build1_time% seconds
echo Incremental build time: %build2_time% seconds
echo Test time: %test_time% seconds
echo Total time: %config_time% + %build1_time% + %build2_time% + %test_time% seconds

cd ..
exit /b 0

:: Time difference function
:time_diff
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