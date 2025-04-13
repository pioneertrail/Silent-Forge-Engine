# Enable building examples (optional, can be toggled)
option(BUILD_EXAMPLES "Build example applications" ON)

if(BUILD_EXAMPLES)
    # Add the instanced mesh example
    add_executable(instanced_mesh_example
        instanced_mesh_example.cpp
    )
    
    # Link with the rendering library
    if(NOT SKIP_DEPENDENCIES)
  if(NOT SKIP_DEPENDENCIES)
  target_link_libraries(instanced_mesh_example
        PRIVATE
        rendering
        glad
        glfw
        OpenGL::GL
        glm::glm
    )
endif()
endif()
    
    # Include directories
    target_include_directories(instanced_mesh_example PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
    )
    
    # Set output directory for examples
    set_target_properties(instanced_mesh_example
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
    )
    
    # Install examples if needed
    install(TARGETS instanced_mesh_example
        RUNTIME DESTINATION bin/examples
    )
endif() 