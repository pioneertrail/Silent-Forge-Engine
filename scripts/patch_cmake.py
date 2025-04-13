#!/usr/bin/env python
"""
This script patches the CMakeLists.txt file to support build benchmarking.
"""
import re
import os
import shutil

# Create backup if it doesn't exist
if not os.path.exists('CMakeLists.txt.bak'):
    print("Creating backup in CMakeLists.txt.bak")
    shutil.copy('CMakeLists.txt', 'CMakeLists.txt.bak')

# Read the file
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# Add benchmark options at the top
benchmark_options = """# Benchmark options
option(USE_PRECOMPILED_HEADERS "Use precompiled headers for faster builds" OFF)
option(SKIP_DEPENDENCIES "Skip most dependencies for minimal build" OFF)
option(USE_OPENGL "Include OpenGL dependency" OFF)
option(USE_GLM "Include GLM dependency" OFF)
option(USE_SDL2 "Include SDL2 dependency" OFF)
option(USE_JSON "Include JSON dependency" OFF)
option(USE_GLFW "Include GLFW dependency" OFF)
"""

# Insert options after cmake_minimum_required
content = re.sub(
    r'(cmake_minimum_required\(.*?\).*?\n)',
    r'\1\n' + benchmark_options,
    content,
    flags=re.DOTALL
)

# Wrap find_package statements with conditional blocks
content = re.sub(
    r'find_package\(OpenGL REQUIRED\)',
    r'if(NOT SKIP_DEPENDENCIES AND (USE_OPENGL OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(OpenGL REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(SDL2 CONFIG REQUIRED.*?\)',
    r'if(NOT SKIP_DEPENDENCIES AND (USE_SDL2 OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(SDL2 CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(nlohmann_json CONFIG REQUIRED\)',
    r'if(NOT SKIP_DEPENDENCIES AND (USE_JSON OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(nlohmann_json CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(glm CONFIG REQUIRED\)',
    r'if(NOT SKIP_DEPENDENCIES AND (USE_GLM OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(glm CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(Catch2 CONFIG REQUIRED\)',
    r'if(NOT SKIP_DEPENDENCIES AND DEFINED CMAKE_TOOLCHAIN_FILE)\n  find_package(Catch2 CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(GLFW3 CONFIG REQUIRED\)',
    r'if(NOT SKIP_DEPENDENCIES AND (USE_GLFW OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(GLFW3 CONFIG REQUIRED)\nendif()',
    content
)

# Add precompiled headers conditionally
pch_block = """
# Enable precompiled headers if requested
if(USE_PRECOMPILED_HEADERS)
  target_precompile_headers(SilentForgeEngine PRIVATE
    <vector>
    <string>
    <memory>
    <algorithm>
    <functional>
    <unordered_map>
    <glm/glm.hpp>
    <glad/glad.h>
  )
endif()
"""

# Find the position to insert precompiled headers (after target_link_libraries)
target_link_pattern = r'target_link_libraries\(SilentForgeEngine PRIVATE.*?\)'
match = re.search(target_link_pattern, content, re.DOTALL)
if match:
    pos = match.end()
    content = content[:pos] + '\n' + pch_block + content[pos:]

# Write the patched file
with open('CMakeLists.txt', 'w') as f:
    f.write(content)

print("CMakeLists.txt has been patched for benchmarking!")
print("Run one of the benchmark scripts to test build performance.") 