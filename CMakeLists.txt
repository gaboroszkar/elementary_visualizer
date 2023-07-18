cmake_minimum_required(VERSION 3.22)

project(elementary_visualizer)

option(BUILD_TESTING "Build testing" ON)

# Generate compile_commands.json.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(add_external_subdirectories)
    # External libraries are statically linked.
    set(BUILD_SHARED_LIBS OFF)

    # expected-lite.
    set(EXPECTED_LITE_DIR ${PROJECT_SOURCE_DIR}/external/expected/)
    set(EXPECTED_BUILD_TESTS OFF)
    add_subdirectory(${EXPECTED_LITE_DIR})
    target_link_libraries(${PROJECT_NAME} expected)

    # GLFW.
    set(GLFW_DIR ${PROJECT_SOURCE_DIR}/external/glfw/)
    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
    add_subdirectory(${GLFW_DIR})
    target_link_libraries(${PROJECT_NAME} glfw)

    # glad.
    set(GL_VERSION_MAJOR 4)
    set(GL_VERSION_MINOR 6)
    set(GLAD_CORE_LIBRARY glad_gl_core_${GL_VERSION_MAJOR}${GL_VERSION_MINOR})
    set(GLAD_DIR ${PROJECT_SOURCE_DIR}/external/glad/)
    add_subdirectory(${GLAD_DIR}/cmake)
    glad_add_library(${GLAD_CORE_LIBRARY} STATIC API
        gl:core=${GL_VERSION_MAJOR}.${GL_VERSION_MINOR})
    # If BUILD_SHARED_LIBS is OFF, the glad is built without the -fPIC option,
    # which is fine if we statically link this to an executable,
    # but not when this library is built into an other library which will be
    # dynamically linked to an executable.
    set_property(TARGET ${GLAD_CORE_LIBRARY} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        GL_VERSION_MAJOR=${GL_VERSION_MAJOR}
        GL_VERSION_MINOR=${GL_VERSION_MINOR})
    target_link_libraries(${PROJECT_NAME} ${GLAD_CORE_LIBRARY})

    # glm.
    set(GLM_DIR ${PROJECT_SOURCE_DIR}/external/glm/)
    add_subdirectory(${GLM_DIR})
    target_link_libraries(${PROJECT_NAME} glm::glm)
endfunction()

# Add target and sources.
set(SOURCES
    src/elementary_visualizer.cpp
    src/entity.cpp
    src/gl_resources.cpp
    src/gl_shader_program.cpp
    src/glfw_resources.cpp
    src/scene.cpp
    src/shader_sources_quad.cpp
    src/shader_sources_depth_peeling.cpp
    src/shader_sources_linesegments.cpp
    src/visuals.cpp
    src/window.cpp
)
add_library(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)

# Add include directories;
# applies only to this subproject.
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

add_external_subdirectories()

# Add include directories to the targets using this library.
# Note, because INTERFACE is set, this is applied
# only the targets using this library will be affected, not this
# library. If it's not the case, use PRIVATE or PUBLIC.
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Make formatting run before we build the project.
add_dependencies(${PROJECT_NAME} clangformat)

file(
    GLOB_RECURSE
    SOURCES_TO_FORMAT
    include/*.c
    include/*.cpp
    include/*.h
    include/*.hpp
    src/*.c
    src/*.cpp
    src/*.h
    src/*.hpp
    tests/*.c
    tests/*.cpp
    tests/*.h
    tests/*.hpp
)
add_custom_target(
    clangformat
    ALL
    COMMAND /usr/bin/clang-format
    -i
    ${SOURCES_TO_FORMAT}
)
