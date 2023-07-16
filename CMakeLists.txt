cmake_minimum_required(VERSION 3.22)

project(elementary_visualizer)

# Generate compile_commands.json.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add target and sources.
set(SOURCES
    src/scene.cpp
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

# Add include directories to the targets using this library.
# Note, because INTERFACE is set, this is applied
# only the targets using this library will be affected, not this
# library. If it's not the case, use PRIVATE or PUBLIC.
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

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
)
add_custom_target(
    clangformat
    ALL
    COMMAND /usr/bin/clang-format
    -i
    ${SOURCES_TO_FORMAT}
)
