@echo off
setlocal enabledelayedexpansion

echo === Incremental Build Process ===
echo This script will incrementally add dependencies until a full build is achieved

:: Setup environment
set PATH=%PATH%;C:\msys64\mingw64\bin;C:\Strawberry\c\bin

:: Set common flags for all builds
set CMAKE_BASE_ARGS=-G "Ninja" -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DUSE_PRECOMPILED_HEADERS=ON

:: Step 1: Core only build (minimal dependencies)
echo.
echo === STEP 1: Core build with minimal dependencies ===
call :run_build_step core_build "-DBUILD_MINIMAL=ON"

:: Step 2: Add OpenGL
echo.
echo === STEP 2: Adding OpenGL dependency ===
call :run_build_step opengl_build "-DBUILD_MINIMAL=ON -DUSE_OPENGL=ON"

:: Step 3: Add glm
echo.
echo === STEP 3: Adding GLM dependency ===
call :run_build_step glm_build "-DBUILD_MINIMAL=ON -DUSE_OPENGL=ON -DUSE_GLM=ON"

:: Step 4: Add SDL2
echo.
echo === STEP 4: Adding SDL2 dependency ===
call :run_build_step sdl2_build "-DBUILD_MINIMAL=ON -DUSE_OPENGL=ON -DUSE_GLM=ON -DUSE_SDL2=ON"

:: Step 5: Add JSON
echo.
echo === STEP 5: Adding JSON dependency ===
call :run_build_step json_build "-DBUILD_MINIMAL=ON -DUSE_OPENGL=ON -DUSE_GLM=ON -DUSE_SDL2=ON -DUSE_JSON=ON"

:: Step 6: Add GLFW
echo.
echo === STEP 6: Adding GLFW dependency ===
call :run_build_step glfw_build "-DBUILD_MINIMAL=ON -DUSE_OPENGL=ON -DUSE_GLM=ON -DUSE_SDL2=ON -DUSE_JSON=ON -DUSE_GLFW=ON"

:: Step 7: Full Build
echo.
echo === STEP 7: Full build with all dependencies ===
call :run_build_step full_build "-DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake"

echo.
echo === Incremental Build Summary ===
echo Core build: !core_time! seconds
echo +OpenGL: !opengl_time! seconds
echo +GLM: !glm_time! seconds
echo +SDL2: !sdl2_time! seconds
echo +JSON: !json_time! seconds
echo +GLFW: !glfw_time! seconds
echo Full build: !full_time! seconds

goto :eof

:run_build_step
set build_dir=%~1
set extra_args=%~2
set start_time=%time%

echo Building in %build_dir%...
if exist %build_dir% rmdir /s /q %build_dir%
mkdir %build_dir%
pushd %build_dir%

echo Configure with: %CMAKE_BASE_ARGS% %extra_args%
cmake .. %CMAKE_BASE_ARGS% %extra_args%

if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed!
    popd
    set %build_dir:~0,-6%_time=FAILED
    exit /b 1
)

echo Building...
cmake --build . -j %NUMBER_OF_PROCESSORS%

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    popd
    set %build_dir:~0,-6%_time=FAILED
    exit /b 1
)

popd
set end_time=%time%

call :calc_time_diff "%start_time%" "%end_time%" diff
set %build_dir:~0,-6%_time=!diff!

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