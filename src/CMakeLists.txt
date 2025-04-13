# Core library configuration
set(CORE_SOURCES
    core/SceneNode.cpp
    core/Camera.cpp
    core/InputManager.cpp
    core/Gamepad.cpp
)

# Create core library
add_library(SilentForgeCore STATIC ${CORE_SOURCES})

# Set include directories
target_include_directories(SilentForgeCore
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/third_party
    ${SDL2_INCLUDE_DIRS}
)

# Set compile definitions
target_compile_definitions(SilentForgeCore
    PUBLIC
    GLFW_INCLUDE_NONE
)

# Link core library with required libraries
if(NOT SKIP_DEPENDENCIES)
  if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(SilentForgeCore
    PUBLIC
    SDL2::SDL2
    nlohmann_json::nlohmann_json
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
)
endif()
endif()

# Set library properties
set_target_properties(SilentForgeCore PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE ON
)

# Install targets
install(TARGETS SilentForgeCore
    EXPORT SilentForgeCoreTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
) 