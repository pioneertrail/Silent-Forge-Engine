cmake_minimum_required(VERSION 3.10)
project(SilentForgeCoreTests)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(Catch2 REQUIRED)
find_package(GTest REQUIRED)

# Add test executable
add_executable(core_test
    core_test.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Gamepad.cpp
    ${CMAKE_SOURCE_DIR}/src/core/config.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Camera.cpp
)

# Include directories
target_include_directories(core_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/third_party
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${CATCH2_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(core_test PRIVATE
    ${SDL2_LIBRARIES}
    Catch2::Catch2
    GTest::GTest
    GTest::Main
)

# Set C++ standard
set_target_properties(core_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Copy resources
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add compile definitions to ensure correct include order
target_compile_definitions(core_test PRIVATE
    GLFW_INCLUDE_NONE
) 