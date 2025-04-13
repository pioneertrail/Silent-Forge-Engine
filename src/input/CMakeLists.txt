# Input module CMakeLists.txt

set(INPUT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gamepad.cpp
)

set(INPUT_HEADERS
    ${CMAKE_SOURCE_DIR}/include/input/Gamepad.hpp
)

# Create a static library for the Input module
add_library(input STATIC ${INPUT_SOURCES} ${INPUT_HEADERS})

# Set include directories
target_include_directories(input
    PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/third_party
)

# Link dependencies
target_link_libraries(input
    PUBLIC
    core
    ${SDL2_LIBRARY}
)

# Add input library to the main sources list
set(ALL_SOURCES ${ALL_SOURCES} ${INPUT_SOURCES} PARENT_SCOPE) 