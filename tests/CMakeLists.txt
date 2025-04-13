cmake_minimum_required(VERSION 3.16)
project(SilentForgeTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(ENABLE_SANITIZERS "Enable AddressSanitizer and ThreadSanitizer" OFF)
option(ENABLE_COVERAGE "Enable test coverage reporting" OFF)
option(ENABLE_JSON_SUPPORT "Enable JSON configuration support" ON)
option(ENABLE_RENDERING "Enable rendering features" ON)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis tools" ON)
option(ENABLE_CPPCHECK "Enable cppcheck analysis" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis" OFF)

# Include GTest module
include(GoogleTest)

# Use installed packages instead of FetchContent
find_package(Catch2 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Core test sources
set(TEST_SOURCES
    core/core_test_app.cpp
    core/Gamepad_test.cpp
    core/SceneNode_test.cpp
    core/InputManager_test.cpp
    core/Camera_test.cpp
)

# Rendering test sources
set(RENDERING_TEST_SOURCES
    rendering/test_instanced_mesh.cpp
    rendering/Shader_test.cpp
    rendering/Framebuffer_test.cpp
    rendering/Renderer_test.cpp
)

# Create core test executable
add_executable(core_tests ${TEST_SOURCES})

# Create rendering test executable
add_executable(rendering_tests ${RENDERING_TEST_SOURCES})

# Set include directories for core tests
target_include_directories(core_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include/third_party
)

# Set include directories for rendering tests
target_include_directories(rendering_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include/third_party
    ${CMAKE_SOURCE_DIR}/lib/glm
)

# Link libraries for core tests
if(NOT SKIP_DEPENDENCIES)
  if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(core_tests PRIVATE
    SilentForgeCore
    GTest::gtest
    GTest::gtest_main
    Catch2::Catch2
    SDL2::SDL2
    nlohmann_json::nlohmann_json
)
endif()
endif()

# Link libraries for rendering tests
if(NOT SKIP_DEPENDENCIES)
  if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(rendering_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    rendering
    glad
    OpenGL::GL
    glfw
)
endif()
endif()

# Set output directories
set_target_properties(core_tests rendering_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

# Add tests to CTest
add_test(NAME core_tests COMMAND core_tests)
add_test(NAME rendering_tests COMMAND rendering_tests)

# Copy test fixtures
file(GLOB TEST_FIXTURES "${CMAKE_SOURCE_DIR}/fixtures/*")
add_custom_command(TARGET core_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/fixtures $<TARGET_FILE_DIR:core_tests>/fixtures
    COMMENT "Copying test fixtures to build directory")

# Enable test discovery for Google Test
gtest_discover_tests(core_tests)
gtest_discover_tests(rendering_tests)

# Enable sanitizers for debug builds
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(core_tests PRIVATE -fsanitize=address,thread -fno-omit-frame-pointer)
    target_link_options(core_tests PRIVATE -fsanitize=address,thread)
    target_compile_options(rendering_tests PRIVATE -fsanitize=address,thread -fno-omit-frame-pointer)
    target_link_options(rendering_tests PRIVATE -fsanitize=address,thread)
endif()

# Enable coverage reporting
if(ENABLE_COVERAGE)
    target_compile_options(core_tests PRIVATE --coverage)
    target_link_options(core_tests PRIVATE --coverage)
    target_compile_options(rendering_tests PRIVATE --coverage)
    target_link_options(rendering_tests PRIVATE --coverage)
    
    add_custom_target(coverage
        COMMAND lcov --capture --directory . --output-file coverage.info
        COMMAND genhtml coverage.info --output-directory coverage_report
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report"
    )
endif()

# Precompile headers for faster builds
target_precompile_headers(core_tests PRIVATE
    <vector>
    <string>
    <memory>
    <glm/glm.hpp>
)

# Add compile definitions
target_compile_definitions(core_tests PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# Set compiler warnings
if(MSVC)
    target_compile_options(core_tests PRIVATE /W4 /WX)
    target_compile_options(rendering_tests PRIVATE /W4 /WX)
else()
    target_compile_options(core_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(rendering_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Enable test configuration
enable_testing()

# Add test executable
add_executable(test_instanced_mesh
    rendering/test_instanced_mesh.cpp
)

# Set include directories for test_instanced_mesh
target_include_directories(test_instanced_mesh
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include/third_party
)

# Link libraries
if(NOT SKIP_DEPENDENCIES)
  if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(test_instanced_mesh
    PRIVATE
    rendering
    glad
    OpenGL::GL
    glfw
    GTest::gtest
    GTest::gtest_main
)
endif()
endif()

# Add test
gtest_discover_tests(test_instanced_mesh)

# Set output directory
set_target_properties(test_instanced_mesh
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
)

# Static Analysis Configuration
if(ENABLE_STATIC_ANALYSIS)
    # cppcheck configuration
    if(ENABLE_CPPCHECK)
        find_program(CPPCHECK cppcheck)
        if(CPPCHECK)
            set(CPPCHECK_TEMPLATE "
                {file}:{line}:{column}: {severity}:{inconclusive:inconclusive:} {message} [{id}]
                {code}
                {callstack}
            ")
            add_custom_target(cppcheck
                COMMAND ${CPPCHECK}
                    --enable=all
                    --std=c++17
                    --suppress=missingIncludeSystem
                    --suppress=unusedFunction
                    --template="${CPPCHECK_TEMPLATE}"
                    --project=${CMAKE_BINARY_DIR}/compile_commands.json
                    --output-file=cppcheck_report.txt
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running cppcheck static analysis"
            )
        endif()
    endif()

    # clang-tidy configuration
    if(ENABLE_CLANG_TIDY)
        find_program(CLANG_TIDY clang-tidy)
        if(CLANG_TIDY)
            set(CLANG_TIDY_CHECKS
                -*
                clang-analyzer-*
                modernize-*
                performance-*
                readability-*
                -modernize-use-trailing-return-type
                -readability-identifier-length
            )
            add_custom_target(clang_tidy
                COMMAND ${CLANG_TIDY}
                    -p ${CMAKE_BINARY_DIR}
                    -checks=${CLANG_TIDY_CHECKS}
                    -header-filter=.*
                    ${CMAKE_SOURCE_DIR}/src/*.cpp
                    ${CMAKE_SOURCE_DIR}/tests/*.cpp
                    --export-fixes=clang_tidy_fixes.yaml
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running clang-tidy analysis"
            )
        endif()
    endif()

    # Combined analysis target
    add_custom_target(static_analysis
        DEPENDS cppcheck clang_tidy
        COMMENT "Running all static analysis tools"
    )
endif()

# Add analysis targets to all target
add_custom_target(all_analysis
    DEPENDS static_analysis coverage
    COMMENT "Running all analysis tools"
) 