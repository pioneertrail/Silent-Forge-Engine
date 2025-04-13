cmake_minimum_required(VERSION 3.16) 
project(MiniBenchmark LANGUAGES CXX) 
 
option(USE_PRECOMPILED_HEADERS "Use precompiled headers" OFF) 
 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
 
add_executable(mini_app src/main.cpp) 
 
if(USE_PRECOMPILED_HEADERS) 
  target_precompile_headers(mini_app PRIVATE 
    <vector> 
    <string> 
    <iostream> 
    <algorithm> 
  ) 
endif() 
