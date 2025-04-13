add_library(rendering STATIC
    InstancedMesh.cpp
)

target_include_directories(rendering
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/third_party
)

if(NOT SKIP_DEPENDENCIES)
  if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(rendering
    PUBLIC
    glad
    glfw
    OpenGL::GL
    glm::glm
)
endif()
endif() 