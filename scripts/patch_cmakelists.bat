@echo off
echo === Patching CMakeLists.txt for benchmarking ===

:: Create backup of original
if not exist CMakeLists.txt.bak (
    copy CMakeLists.txt CMakeLists.txt.bak
    echo Created backup in CMakeLists.txt.bak
)

:: Add our benchmark options to the CMakeLists.txt
echo Adding benchmark options to CMakeLists.txt...

:: Create a temporary file with our changes
echo # CMakeLists.txt > CMakeLists.new
echo cmake_minimum_required(VERSION 3.16) >> CMakeLists.new
echo. >> CMakeLists.new
echo # Benchmark options >> CMakeLists.new
echo option(USE_PRECOMPILED_HEADERS "Use precompiled headers for faster builds" OFF) >> CMakeLists.new
echo option(SKIP_DEPENDENCIES "Skip most dependencies for minimal build" OFF) >> CMakeLists.new
echo option(USE_OPENGL "Include OpenGL dependency" OFF) >> CMakeLists.new
echo option(USE_GLM "Include GLM dependency" OFF) >> CMakeLists.new
echo option(USE_SDL2 "Include SDL2 dependency" OFF) >> CMakeLists.new
echo option(USE_JSON "Include JSON dependency" OFF) >> CMakeLists.new
echo option(USE_GLFW "Include GLFW dependency" OFF) >> CMakeLists.new
echo. >> CMakeLists.new

:: Append the rest of the original file, skipping the first line
findstr /v /b "# CMakeLists.txt" CMakeLists.txt.bak >> CMakeLists.new

:: Replace the original file
move /y CMakeLists.new CMakeLists.txt

:: Now patch the dependency sections to be conditional
echo Patching dependency sections with Python...

:: Create a simple Python script for patching
(
echo import re
echo.
echo with open('CMakeLists.txt', 'r') as f:
echo     content = f.read()
echo.
echo # Wrap find_package statements with conditional blocks
echo content = re.sub(r'find_package\(OpenGL REQUIRED\)',
echo                  r'if(NOT SKIP_DEPENDENCIES AND (USE_OPENGL OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(OpenGL REQUIRED)\nendif()',
echo                  content)
echo.
echo content = re.sub(r'find_package\(SDL2 CONFIG REQUIRED.*?\)',
echo                  r'if(NOT SKIP_DEPENDENCIES AND (USE_SDL2 OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(SDL2 CONFIG REQUIRED)\nendif()',
echo                  content)
echo.
echo content = re.sub(r'find_package\(nlohmann_json CONFIG REQUIRED\)',
echo                  r'if(NOT SKIP_DEPENDENCIES AND (USE_JSON OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(nlohmann_json CONFIG REQUIRED)\nendif()',
echo                  content)
echo.
echo content = re.sub(r'find_package\(glm CONFIG REQUIRED\)',
echo                  r'if(NOT SKIP_DEPENDENCIES AND (USE_GLM OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(glm CONFIG REQUIRED)\nendif()',
echo                  content)
echo.
echo content = re.sub(r'find_package\(Catch2 CONFIG REQUIRED\)',
echo                  r'if(NOT SKIP_DEPENDENCIES AND DEFINED CMAKE_TOOLCHAIN_FILE)\n  find_package(Catch2 CONFIG REQUIRED)\nendif()',
echo                  content)
echo.
echo content = re.sub(r'find_package\(GLFW3 CONFIG REQUIRED\)',
echo                  r'if(NOT SKIP_DEPENDENCIES AND (USE_GLFW OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(GLFW3 CONFIG REQUIRED)\nendif()',
echo                  content)
echo.
echo # Add precompiled headers conditionally
echo pch_block = '''
echo # Enable precompiled headers if requested
echo if(USE_PRECOMPILED_HEADERS)
echo   target_precompile_headers(SilentForgeEngine PRIVATE
echo     ^<vector^>
echo     ^<string^>
echo     ^<memory^>
echo     ^<algorithm^>
echo     ^<functional^>
echo     ^<unordered_map^>
echo     ^<glm/glm.hpp^>
echo     ^<glad/glad.h^>
echo   )
echo endif()
echo '''
echo.
echo # Find the position to insert precompiled headers (after target_link_libraries)
echo target_link_pattern = r'target_link_libraries\(SilentForgeEngine PRIVATE.*?\)'
echo match = re.search(target_link_pattern, content, re.DOTALL)
echo if match:
echo     pos = match.end()
echo     content = content[:pos] + '\n' + pch_block + content[pos:]
echo.
echo with open('CMakeLists.txt', 'w') as f:
echo     f.write(content)
) > patch_cmake.py

:: Run the Python script
python patch_cmake.py
if %ERRORLEVEL% NEQ 0 (
    echo Failed to patch CMakeLists.txt with Python
    exit /b 1
)

echo CMakeLists.txt has been patched for benchmarking!
echo Run one of the benchmark scripts to test build performance. 