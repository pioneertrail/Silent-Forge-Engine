cmake_minimum_required(VERSION 3.10)
project(SilentForgeRenderingTests)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(Catch2 REQUIRED)
find_package(GTest REQUIRED)

# Add test executable
add_executable(rendering_test
    texture_test.cpp
    GLContextHelper_test.cpp
    Mesh_test.cpp
    InstancedMesh_test.cpp
    Shader_test.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/Texture.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/GLContextHelper.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/InstancedMesh.cpp
)

# Include directories
target_include_directories(rendering_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/third_party
    ${CMAKE_SOURCE_DIR}/lib/stb
    ${OPENGL_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${CATCH2_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
)

# Link libraries
if(NOT SKIP_DEPENDENCIES)
  if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(rendering_test PRIVATE
    glad
    glfw
    OpenGL::GL
    Catch2::Catch2
    GTest::GTest
    GTest::Main
)
endif()
endif()

# Set C++ standard
set_target_properties(rendering_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Copy resources
file(COPY ${CMAKE_SOURCE_DIR}/resources/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Add compile definitions to ensure correct include order
target_compile_definitions(rendering_test PRIVATE
    GLFW_INCLUDE_NONE
)

# Add coverage support if enabled
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(rendering_test PRIVATE --coverage)
        target_link_libraries(rendering_test PRIVATE --coverage)
    elseif(MSVC)
        message(WARNING "Coverage reporting is not supported with MSVC. Use GCC or Clang for coverage.")
    else()
        message(WARNING "Coverage reporting is not supported with this compiler.")
    endif()
endif() 