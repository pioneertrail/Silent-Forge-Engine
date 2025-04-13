cmake_minimum_required(VERSION 3.16)
project(FastBuildTest)

# Enable ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
endif()

# Basic C++ settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Simple executable
add_executable(minimal_test main.cpp)

# Precompiled headers
target_precompile_headers(minimal_test PRIVATE 
  <vector>
  <string>
  <iostream>
)

# Testing
enable_testing()
add_test(NAME TestRunnable COMMAND minimal_test) 