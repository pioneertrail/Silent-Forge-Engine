#!/usr/bin/env python
"""
This script modifies the CMakeLists.txt to handle dependency flags properly.
"""
import os
import re
import shutil

# Backup original if not already done
if not os.path.exists('CMakeLists.txt.bak'):
    shutil.copy('CMakeLists.txt', 'CMakeLists.txt.bak')
    print("Created backup at CMakeLists.txt.bak")
else:
    # Restore from backup to start clean
    shutil.copy('CMakeLists.txt.bak', 'CMakeLists.txt')
    print("Restored from backup")

# Read in the CMakeLists.txt file
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# Add benchmark options at the top (right after cmake_minimum_required)
options = """
# Benchmark options
option(USE_PRECOMPILED_HEADERS "Use precompiled headers for faster builds" OFF)
option(SKIP_DEPENDENCIES "Skip most dependencies for minimal build" OFF)
option(USE_OPENGL "Include OpenGL dependency" OFF)
option(USE_GLM "Include GLM dependency" OFF)
option(USE_SDL2 "Include SDL2 dependency" OFF)
option(USE_JSON "Include JSON dependency" OFF)
option(USE_GLFW "Include GLFW dependency" OFF)

"""

# Insert options after the project declaration
content = re.sub(
    r'(project\(.*?\))',
    r'\1\n' + options,
    content
)

# Fix find_package calls to support skipping
content = re.sub(
    r'find_package\(OpenGL REQUIRED\)',
    r'if(NOT SKIP_DEPENDENCIES)\n  find_package(OpenGL REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(SDL2 .*?\)',
    r'if(NOT SKIP_DEPENDENCIES)\n  find_package(SDL2 CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(nlohmann_json .*?\)',
    r'if(NOT SKIP_DEPENDENCIES)\n  find_package(nlohmann_json CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(GTest .*?\)',
    r'if(NOT SKIP_DEPENDENCIES AND BUILD_TESTING)\n  find_package(GTest CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(glm .*?\)',
    r'if(NOT SKIP_DEPENDENCIES)\n  find_package(glm CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(Catch2 .*?\)',
    r'if(NOT SKIP_DEPENDENCIES AND BUILD_TESTING)\n  find_package(Catch2 CONFIG REQUIRED)\nendif()',
    content
)

content = re.sub(
    r'find_package\(GLFW3 .*?\)',
    r'if(NOT SKIP_DEPENDENCIES)\n  find_package(GLFW3 CONFIG REQUIRED)\nendif()',
    content
)

# Fix SDL2 variables - only use them if not skipping dependencies
content = re.sub(
    r'(set\(SDL2_LIBRARIES .*?\))',
    r'if(NOT SKIP_DEPENDENCIES)\n  \1\nendif()',
    content
)

# Fix the target_link_libraries section to be conditional
new_target_link = """# Link libraries
target_link_libraries(SilentForgeEngine PRIVATE 
    glad
)

if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(SilentForgeEngine PRIVATE
    $<$<NOT:$<BOOL:${SKIP_DEPENDENCIES}>>:OpenGL::GL>
    $<$<NOT:$<BOOL:${SKIP_DEPENDENCIES}>>:glm::glm>
    $<$<NOT:$<BOOL:${SKIP_DEPENDENCIES}>>:glfw>
    $<$<NOT:$<BOOL:${SKIP_DEPENDENCIES}>>:${SDL2_LIBRARY}>
    $<$<NOT:$<BOOL:${SKIP_DEPENDENCIES}>>:${SDL2MAIN_LIBRARY}>
    $<$<NOT:$<BOOL:${SKIP_DEPENDENCIES}>>:nlohmann_json::nlohmann_json>
  )
endif()

# Only use Win32 libraries if not skipping dependencies
if(WIN32 AND NOT SKIP_DEPENDENCIES)
  target_link_libraries(SilentForgeEngine PRIVATE gdi32 user32)
endif()"""

# Replace the target_link_libraries section
content = re.sub(
    r'# Link libraries.*?endif\(\)',
    new_target_link,
    content,
    flags=re.DOTALL
)

# Add precompiled headers section
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

# Find a good place to insert PCH (after the includes)
content = re.sub(
    r'(target_include_directories\(SilentForgeEngine.*?\))',
    r'\1\n' + pch_block,
    content,
    flags=re.DOTALL
)

# Make subdirectories conditional for examples and tests
content = re.sub(
    r'(add_subdirectory\(src/rendering\))',
    r'\1\n\nif(NOT SKIP_DEPENDENCIES)',
    content
)

content = re.sub(
    r'(add_subdirectory\(src/examples\))',
    r'  \1\nendif()',
    content
)

# Write the modified content back to CMakeLists.txt
with open('CMakeLists.txt', 'w') as f:
    f.write(content)

print("CMakeLists.txt has been fixed for benchmarking!")
print("You can now run scripts/simple_benchmark.bat to test build optimizations.") 