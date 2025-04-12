cmake_minimum_required(VERSION 3.16)
project(SilentForgeEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find dependencies
find_package(OpenGL REQUIRED)

# Add third-party libraries
# GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(lib/glfw)

# GLM (header-only)
include_directories(lib/glm)

# GLAD
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

# STB (header-only)
include_directories(lib/stb)

# Source files
file(GLOB CORE_SOURCES "src/core/*.cpp")
file(GLOB RENDERING_SOURCES "src/rendering/*.cpp")
set(SOURCES 
    src/main.cpp 
    ${CORE_SOURCES} 
    ${RENDERING_SOURCES}
)

# Header files
file(GLOB CORE_HEADERS "include/core/*.hpp")
file(GLOB RENDERING_HEADERS "include/rendering/*.hpp")
set(HEADERS 
    ${CORE_HEADERS} 
    ${RENDERING_HEADERS}
)

# Executable
add_executable(SilentForgeEngine ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(SilentForgeEngine PRIVATE include)

# Link libraries
target_link_libraries(SilentForgeEngine PRIVATE glad glfw OpenGL::GL)

# Platform-specific settings
if(WIN32)
    target_link_libraries(SilentForgeEngine PRIVATE gdi32 user32)
elseif(APPLE)
    target_link_libraries(SilentForgeEngine PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
endif()

# Copy assets to build directory
add_custom_command(TARGET SilentForgeEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:SilentForgeEngine>/shaders
    COMMENT "Copying shaders to build directory")

add_custom_command(TARGET SilentForgeEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:SilentForgeEngine>/assets
    COMMENT "Copying assets to build directory") 