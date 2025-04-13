# Basic Window Example
cmake_minimum_required(VERSION 3.16)
project(SFE_BasicWindow LANGUAGES CXX C)

# Find the SilentForge engine
find_package(SilentForgeEngine REQUIRED)

# This is a placeholder - example will be implemented when the engine compiles correctly
# add_executable(basic_window main.cpp)
# target_link_libraries(basic_window PRIVATE SilentForgeEngine)

# Create a README explaining this is a placeholder
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/README.md 
"# Basic Window Example

This example will demonstrate how to create a basic window using the SilentForge Engine.

## Status
This example is currently a placeholder and will be implemented once the engine compilation issues are resolved.

## Expected Features
- Window creation
- Basic event handling
- Window closing
- Clearing the screen
") 