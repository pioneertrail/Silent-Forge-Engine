# Core module CMakeLists.txt

set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Gamepad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SceneNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InputManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WindowManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp
)

set(CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/core/Gamepad.hpp
    ${CMAKE_SOURCE_DIR}/include/core/SceneNode.hpp
    ${CMAKE_SOURCE_DIR}/include/core/InputManager.hpp
    ${CMAKE_SOURCE_DIR}/include/core/Camera.hpp
    ${CMAKE_SOURCE_DIR}/include/core/Application.hpp
    ${CMAKE_SOURCE_DIR}/include/core/WindowManager.hpp
    ${CMAKE_SOURCE_DIR}/include/core/config.h
)

# Create a static library for the Core module
add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

# Set include directories
target_include_directories(core
    PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/third_party
)

# Link dependencies
target_link_libraries(core
    PUBLIC
    ${GLAD_LIBRARIES}
    glfw
    glm
    OpenGL::GL
    ${SDL2_LIBRARY}
)

# Add core library to the main sources list
set(ALL_SOURCES ${ALL_SOURCES} ${CORE_SOURCES} PARENT_SCOPE) 