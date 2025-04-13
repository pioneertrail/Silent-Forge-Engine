# Rendering module CMakeLists.txt

set(RENDERING_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InstancedMesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLContextHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GLStateCache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Framebuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Frustum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TextRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ShaderManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderPipeline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
)

set(RENDERING_HEADERS
    ${CMAKE_SOURCE_DIR}/include/rendering/Shader.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/Mesh.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/InstancedMesh.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/Texture.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/GLContext.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/GLContextHelper.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/GLStateCache.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/Renderer.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/Framebuffer.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/Frustum.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/TextRenderer.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/ShaderManager.hpp
    ${CMAKE_SOURCE_DIR}/include/rendering/GLIncludes.hpp
)

# Create a static library for the Rendering module
add_library(rendering STATIC ${RENDERING_SOURCES} ${RENDERING_HEADERS})

# Set include directories
target_include_directories(rendering
    PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/third_party
)

# Link dependencies
target_link_libraries(rendering
    PUBLIC
    core
    ${GLAD_LIBRARIES}
    glfw
    glm
    OpenGL::GL
)

# Add rendering library to the main sources list
set(ALL_SOURCES ${ALL_SOURCES} ${RENDERING_SOURCES} PARENT_SCOPE) 