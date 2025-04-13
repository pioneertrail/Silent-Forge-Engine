import re 
import sys 
ECHO is off.
with open('CMakeLists.txt', 'r') as f: 
    content = f.read() 
ECHO is off.
# Wrap find_package statements with conditional blocks 
content = re.sub(r'find_package\(OpenGL REQUIRED\)', 
                 r'if(NOT SKIP_DEPENDENCIES AND (USE_OPENGL OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(OpenGL REQUIRED)\nendif()', 
                 content) 
ECHO is off.
content = re.sub(r'find_package\(SDL2 CONFIG REQUIRED.*?\)', 
                 r'if(NOT SKIP_DEPENDENCIES AND (USE_SDL2 OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(SDL2 CONFIG REQUIRED)\nendif()', 
                 content) 
ECHO is off.
content = re.sub(r'find_package\(nlohmann_json CONFIG REQUIRED\)', 
                 r'if(NOT SKIP_DEPENDENCIES AND (USE_JSON OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(nlohmann_json CONFIG REQUIRED)\nendif()', 
                 content) 
ECHO is off.
content = re.sub(r'find_package\(glm CONFIG REQUIRED\)', 
                 r'if(NOT SKIP_DEPENDENCIES AND (USE_GLM OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(glm CONFIG REQUIRED)\nendif()', 
                 content) 
ECHO is off.
content = re.sub(r'find_package\(Catch2 CONFIG REQUIRED\)', 
                 r'if(NOT SKIP_DEPENDENCIES AND DEFINED CMAKE_TOOLCHAIN_FILE)\n  find_package(Catch2 CONFIG REQUIRED)\nendif()', 
                 content) 
ECHO is off.
content = re.sub(r'find_package\(GLFW3 CONFIG REQUIRED\)', 
                 r'if(NOT SKIP_DEPENDENCIES AND (USE_GLFW OR DEFINED CMAKE_TOOLCHAIN_FILE))\n  find_package(GLFW3 CONFIG REQUIRED)\nendif()', 
                 content) 
ECHO is off.
# Add precompiled headers conditionally 
pch_block = ''' 
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
''' 
ECHO is off.
# Find the position to insert precompiled headers (after target_link_libraries) 
target_link_pattern = r'target_link_libraries\(SilentForgeEngine PRIVATE.*?\)' 
match = re.search(target_link_pattern, content, re.DOTALL) 
if match: 
    pos = match.end() 
    content = content[:pos] + '\n' + pch_block + content[pos:] 
ECHO is off.
with open('CMakeLists.txt', 'w') as f: 
    f.write(content) 
