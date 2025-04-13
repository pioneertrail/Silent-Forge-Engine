# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(SilentForgeEngine LANGUAGES CXX C)

# Build options
option(USE_PRECOMPILED_HEADERS "Use precompiled headers for faster builds" ON)
option(SKIP_DEPENDENCIES "Skip most dependencies for minimal build" OFF)
option(USE_OPENGL "Include OpenGL dependency" ON)
option(USE_GLM "Include GLM dependency" ON)
option(USE_SDL2 "Include SDL2 dependency" ON)
option(USE_JSON "Include JSON dependency" ON)
option(USE_GLFW "Include GLFW dependency" ON)

# Enable ccache if available to speed up compilation
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# MSVC needs special flags for C++17
if(MSVC)
    add_compile_options(/std:c++17)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set vcpkg installation directory
set(VCPKG_INSTALLED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows)
set(CMAKE_PREFIX_PATH ${VCPKG_INSTALLED_DIR})

# Optimize build settings
if(MSVC)
    # MSVC specific optimizations
    add_compile_options(/MP)  # Enable parallel compilation
    add_compile_options(/W4)  # Warning level 4
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2)  # Optimize for speed
    endif()
else()
    # GCC/Clang specific optimizations
    add_compile_options(-Wall -Wextra)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3)  # Optimize for speed
    endif()
endif()

# Find dependencies
if(NOT SKIP_DEPENDENCIES)
    # Find core dependencies
    find_package(OpenGL REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    
    # Find GLM header-only library
    find_path(GLM_INCLUDE_DIR glm/glm.hpp)
    if(NOT GLM_INCLUDE_DIR)
        message(FATAL_ERROR "GLM not found")
    endif()
    
    # Create an interface library for GLM
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIR})
    
    # Find GLFW manually
    find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h)
    find_library(GLFW_LIBRARY NAMES glfw glfw3 glfw3dll)
    
    if(NOT GLFW_INCLUDE_DIR OR NOT GLFW_LIBRARY)
        message(FATAL_ERROR "GLFW not found")
    endif()
    
    # Create an interface library for GLFW
    add_library(glfw INTERFACE)
    target_include_directories(glfw INTERFACE ${GLFW_INCLUDE_DIR})
    target_link_libraries(glfw INTERFACE ${GLFW_LIBRARY})
else()
    message(STATUS "Skipping dependencies (SKIP_DEPENDENCIES=ON)")
    # Create interface libraries for dependencies when skipped
    add_library(OpenGL::GL INTERFACE IMPORTED)
    add_library(glm INTERFACE)
    add_library(glfw INTERFACE)
endif()

# Testing dependencies - try to find Catch2 but don't fail if not found
if(BUILD_TESTING)
    include(CMakeDependentOption)
    option(BUILD_WITH_CATCH2 "Build with Catch2 for testing" ON)
    
    if(BUILD_WITH_CATCH2)
        find_package(Catch2 CONFIG QUIET)
        if(NOT Catch2_FOUND)
            message(STATUS "Catch2 not found - tests requiring Catch2 will be disabled")
            set(BUILD_WITH_CATCH2 OFF)
        endif()
    endif()
endif()

# Find SDL2 using a more reliable method (needed in both cases)
find_path(SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES SDL2)
find_library(SDL2_LIBRARY NAMES SDL2)
find_library(SDL2MAIN_LIBRARY NAMES SDL2main)

if(NOT SDL2_LIBRARY)
    message(FATAL_ERROR "SDL2 library not found")
endif()

# Make SDL2main optional
if(NOT SDL2MAIN_LIBRARY)
    message(WARNING "SDL2main library not found, continuing without it")
    set(SDL2MAIN_LIBRARY "")
endif()

# Find GTest (needed in both cases)
find_path(GTEST_INCLUDE_DIR gtest/gtest.h)
find_library(GTEST_LIBRARY NAMES gtest)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main)

# Add GLAD (generated files in lib/glad)
if(MSVC)
    # For MSVC builds, find the precompiled GLAD library from vcpkg
    find_package(glad CONFIG REQUIRED)
    set(GLAD_LIBRARIES glad::glad)
else()
    # For other builds, compile from source
    set(CMAKE_C_COMPILER ${CMAKE_CXX_COMPILER})
    add_library(glad STATIC lib/glad/src/glad.c)
    target_include_directories(glad PUBLIC lib/glad/include)
    set_target_properties(glad PROPERTIES
        LINKER_LANGUAGE C
        POSITION_INDEPENDENT_CODE ON
    )
    set(GLAD_LIBRARIES glad)
endif()

# Download stb headers if not present
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/third_party/stb/stb_image.h")
    message(STATUS "Downloading stb_image.h...")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/third_party/stb")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/third_party/stb/stb_image.h"
        SHOW_PROGRESS
    )
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/third_party/stb/stb_image_write.h")
    message(STATUS "Downloading stb_image_write.h...")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/third_party/stb")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/third_party/stb/stb_image_write.h"
        SHOW_PROGRESS
    )
endif()

# Initialize all sources variable
set(ALL_SOURCES "")

# Include subdirectories with CMakeLists.txt files
add_subdirectory(src/core)
add_subdirectory(src/rendering)
add_subdirectory(src/input)
add_subdirectory(src/utils)

# Main executable
add_executable(SilentForgeEngine src/app/main.cpp ${ALL_SOURCES})

# Include directories for the project sources
target_include_directories(SilentForgeEngine 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/third_party
    ${SDL2_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIR}
)

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

# Add compile definitions to ensure correct include order
target_compile_definitions(SilentForgeEngine PRIVATE
    GLFW_INCLUDE_NONE
)

# Link module libraries
target_link_libraries(SilentForgeEngine PRIVATE
    core
    rendering
    input
    utils
)

# Link libraries - always link glad
target_link_libraries(SilentForgeEngine PRIVATE ${GLAD_LIBRARIES})

# Conditionally link other dependencies
if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(SilentForgeEngine PRIVATE
    OpenGL::GL
    glm
    glfw
    ${SDL2_LIBRARY}
  )
  
  # Only link SDL2main if found
  if(SDL2MAIN_LIBRARY)
    target_link_libraries(SilentForgeEngine PRIVATE ${SDL2MAIN_LIBRARY})
  endif()
  
  target_link_libraries(SilentForgeEngine PRIVATE
    nlohmann_json::nlohmann_json
  )
  
  # Only use Win32 libraries if not skipping dependencies
  if(WIN32)
    target_link_libraries(SilentForgeEngine PRIVATE gdi32 user32)
  endif()
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

# Add coverage support if enabled
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(SilentForgeEngine PRIVATE --coverage)
        target_link_libraries(SilentForgeEngine PRIVATE --coverage)
    elseif(MSVC)
        message(WARNING "Coverage reporting is not supported with MSVC. Use GCC or Clang for coverage.")
    else()
        message(WARNING "Coverage reporting is not supported with this compiler.")
    endif()
endif()

# Install targets
install(TARGETS SilentForgeEngine core rendering input utils
    EXPORT SilentForgeEngineTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

# Install header files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Export targets
install(EXPORT SilentForgeEngineTargets
    FILE SilentForgeEngineTargets.cmake
    NAMESPACE SilentForgeEngine::
    DESTINATION lib/cmake/SilentForgeEngine
)

# Configure package file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SilentForgeEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SilentForgeEngineConfig.cmake
    INSTALL_DESTINATION lib/cmake/SilentForgeEngine
)

# Write version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SilentForgeEngineConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

# Install package files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SilentForgeEngineConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SilentForgeEngineConfigVersion.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindSilentForgeEngine.cmake
    DESTINATION lib/cmake/SilentForgeEngine
)

# Special handling for skipped dependencies
if(SKIP_DEPENDENCIES)
  message(STATUS "Skipping dependency checks - libraries must be provided manually")
  include_directories(${CMAKE_SOURCE_DIR})
  # Add mock targets for any required dependencies
  if(NOT TARGET OpenGL::GL)
    add_library(OpenGL::GL INTERFACE IMPORTED)
  endif()
endif()
