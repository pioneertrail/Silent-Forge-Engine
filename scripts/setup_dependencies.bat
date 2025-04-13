@echo off
SETLOCAL ENABLEEXTENSIONS
echo Setting up SilentForge dependencies...

REM Determine if we're using MinGW or MSVC
set BUILD_TYPE=msvc
if "%1"=="mingw" (
    set BUILD_TYPE=mingw
    set TRIPLET=x64-mingw-dynamic
) else (
    set TRIPLET=x64-windows
)

echo Build type: %BUILD_TYPE% (Using %TRIPLET%)

REM Set up vcpkg if not already initialized
if not exist vcpkg\vcpkg.exe (
    echo Bootstrapping vcpkg...
    cd vcpkg
    call bootstrap-vcpkg.bat
    cd ..
)

REM Install dependencies
echo Installing dependencies for %TRIPLET%...
vcpkg\vcpkg install ^
    glm:%TRIPLET% ^
    glfw3:%TRIPLET% ^
    glad:%TRIPLET% ^
    stb:%TRIPLET% ^
    nlohmann-json:%TRIPLET% ^
    catch2:%TRIPLET%

if %ERRORLEVEL% NEQ 0 (
    echo Failed to install dependencies!
    exit /b %ERRORLEVEL%
)

echo Dependencies successfully installed!
echo.
echo To build the project run:
if "%BUILD_TYPE%"=="mingw" (
    echo   build_mingw.bat
) else (
    echo   build.bat
)

exit /b 0 