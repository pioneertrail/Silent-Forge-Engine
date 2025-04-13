# Utils module CMakeLists.txt

set(UTILS_SOURCES
    # Add utility source files here when they're created
)

set(UTILS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/utils/log.h
)

# Create a static library for the Utils module
add_library(utils STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})

# Set include directories
target_include_directories(utils
    PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/third_party
)

# Link dependencies
target_link_libraries(utils
    PUBLIC
    # Add dependencies here as needed
)

# Add utils library to the main sources list
set(ALL_SOURCES ${ALL_SOURCES} ${UTILS_SOURCES} PARENT_SCOPE) 