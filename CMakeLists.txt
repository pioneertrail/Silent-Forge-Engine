# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(NewEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find dependencies
find_package(OpenGL REQUIRED)

# Add GLFW (assuming submodule or system-installed)
# Make sure GLFW is placed in lib/glfw or installed system-wide
add_subdirectory(lib/glfw)

# Add GLM (header-only, assuming submodule)
# Make sure GLM is placed in lib/glm or installed system-wide
include_directories(lib/glm)

# Add GLAD (generated files in lib/glad)
# Ensure glad.c and glad.h are placed correctly in lib/glad
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

# Source files
# Use specific GLOBs instead of GLOB_RECURSIVE for compatibility
file(GLOB CORE_SOURCES "src/core/*.cpp")
file(GLOB RENDERING_SOURCES "src/rendering/*.cpp")
set(SOURCES ${CORE_SOURCES} ${RENDERING_SOURCES})

# Header files (optional, but can help IDEs)
file(GLOB CORE_HEADERS "include/core/*.hpp")
file(GLOB RENDERING_HEADERS "include/rendering/*.hpp")
set(HEADERS ${CORE_HEADERS} ${RENDERING_HEADERS})


# Executable
# Add main.cpp explicitly, then the globbed sources and headers
add_executable(NewEngine src/main.cpp ${SOURCES} ${HEADERS})

# Include directories for the project sources
target_include_directories(NewEngine PRIVATE include)

# Link libraries
# Ensure GLFW library target name is correct (e.g., glfw if using add_subdirectory)
# target_link_libraries(NewEngine PRIVATE glad glfw OpenGL::GL) # Placeholder, adjust based on GLFW integration

# Example linking if GLFW is added via add_subdirectory:
target_link_libraries(NewEngine PRIVATE glad glfw OpenGL::GL)


# Platform-specific settings (Example - may need adjustment)
if (WIN32)
    # Link necessary Windows libraries if needed
    # target_link_libraries(NewEngine PRIVATE gdi32 user32 shell32) # Example
elseif (APPLE)
    # Link necessary macOS frameworks if needed
    # target_link_libraries(NewEngine PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit") # Example
endif()

# Copy shaders to build directory (Optional, but helpful)
# This assumes your executable runs from CMAKE_RUNTIME_OUTPUT_DIRECTORY
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/shaders/*")
add_custom_command(TARGET NewEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:NewEngine>/shaders
    COMMENT "Copying shaders to build directory")

# Build instructions comment (for reference)
# 1. mkdir build && cd build
# 2. cmake .. [options] # e.g., -DGLFW_BUILD_DOCS=OFF if using GLFW submodule
# 3. cmake --build .
# 4. Run ./bin/NewEngine (or .\bin\NewEngine.exe on Windows) 