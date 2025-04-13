# Examples CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(SilentForgeExamples LANGUAGES CXX C)

# Set the path to the SilentForge Engine
set(SILENTFORGE_ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Add the engine as a subdirectory
add_subdirectory(${SILENTFORGE_ENGINE_DIR} ${CMAKE_BINARY_DIR}/silentforge)

# Function to add an example
function(add_example EXAMPLE_NAME)
    # Add the example subdirectory if it exists and has a CMakeLists.txt
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}/CMakeLists.txt)
        add_subdirectory(${EXAMPLE_NAME})
    endif()
endfunction()

# Add examples
add_example(basic_window)
add_example(triangle_rendering)
add_example(texture_loading)

# Additional examples can be added here
# add_example(model_loading)
# add_example(instanced_rendering)
# etc. 