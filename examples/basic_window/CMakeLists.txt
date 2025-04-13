# Basic Window Example
cmake_minimum_required(VERSION 3.16)
project(SFE_BasicWindow LANGUAGES CXX C)

# Option to use installed package or direct source
option(USE_INSTALLED_ENGINE "Use installed SilentForgeEngine instead of source" OFF)

if(USE_INSTALLED_ENGINE)
    # Find the installed SilentForgeEngine package
    find_package(SilentForgeEngine REQUIRED)
    
    # This will be implemented when the engine compiles correctly
    # add_executable(basic_window main.cpp)
    # target_link_libraries(basic_window PRIVATE SilentForgeEngine::Engine)
else()
    # Use the engine from source via the parent CMakeLists.txt
    # Nothing to do here, as the parent already includes the engine
    
    # This will be implemented when the engine compiles correctly
    # add_executable(basic_window main.cpp)
    # target_link_libraries(basic_window PRIVATE SilentForgeEngine)
endif()

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

## Building
You can build this example in two ways:

1. Using the engine source directly (default):
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

2. Using an installed engine package:
```bash
mkdir build && cd build
cmake -DUSE_INSTALLED_ENGINE=ON ..
cmake --build .
```
") 