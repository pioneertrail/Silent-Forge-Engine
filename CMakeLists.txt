# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(SilentForgeEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set vcpkg installation directory
set(VCPKG_INSTALLED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows)
set(CMAKE_PREFIX_PATH ${VCPKG_INSTALLED_DIR})

# Find dependencies
find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# Use FetchContent for dependencies
include(FetchContent)

# GLFW
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw")
    add_subdirectory(lib/glfw)
else()
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()

# GLM
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm")
    include_directories(lib/glm)
else()
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
    )
    FetchContent_MakeAvailable(glm)
endif()

# Add GLAD (generated files in lib/glad)
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

# Add stb directory for stb_image.h
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/stb")
    include_directories(lib/stb)
endif()

# Source files
file(GLOB CORE_SOURCES "src/core/*.cpp")
file(GLOB RENDERING_SOURCES "src/rendering/*.cpp")
set(SOURCES ${CORE_SOURCES} ${RENDERING_SOURCES})

# Header files
file(GLOB CORE_HEADERS "include/core/*.hpp")
file(GLOB RENDERING_HEADERS "include/rendering/*.hpp")
set(HEADERS ${CORE_HEADERS} ${RENDERING_HEADERS})

# Executable
add_executable(SilentForgeEngine src/main.cpp ${SOURCES} ${HEADERS})

# Include directories for the project sources
target_include_directories(SilentForgeEngine PRIVATE include)
target_include_directories(SilentForgeEngine PRIVATE include/third_party)

# Link libraries
target_link_libraries(SilentForgeEngine PRIVATE 
    glad 
    glfw 
    OpenGL::GL 
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

# Platform-specific settings
if (WIN32)
    target_link_libraries(SilentForgeEngine PRIVATE gdi32 user32)
    if (MSVC)
        target_compile_options(SilentForgeEngine PRIVATE /W4)
    endif()
elseif (APPLE)
    # Link necessary macOS frameworks if needed
endif()

# Copy shaders to build directory
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/shaders/*")
add_custom_command(TARGET SilentForgeEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:SilentForgeEngine>/shaders
    COMMENT "Copying shaders to build directory")

# Copy assets to build directory if they exist
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET SilentForgeEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:SilentForgeEngine>/assets
        COMMENT "Copying assets to build directory")
endif()

# Build instructions comment (for reference)
# 1. mkdir build && cd build
# 2. cmake .. [options] # e.g., -DGLFW_BUILD_DOCS=OFF if using GLFW submodule
# 3. cmake --build .
# 4. Run ./bin/SilentForgeEngine (or .\bin\SilentForgeEngine.exe on Windows) 