function(setup_example EXAMPLE_NAME SOURCE_FILE)
    add_executable(${EXAMPLE_NAME} ${SOURCE_FILE})
    set_property(TARGET ${EXAMPLE_NAME} PROPERTY CXX_STANDARD 20)
    target_compile_options(${EXAMPLE_NAME} PRIVATE -Werror -Wall -Wextra)
    target_include_directories(${EXAMPLE_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_link_libraries(${EXAMPLE_NAME} elementary_visualizer)
endfunction()

setup_example(lorenz_attractor lorenz_attractor.cpp)
setup_example(wave wave.cpp)
setup_example(wireframe wireframe.cpp)
setup_example(game_of_life game_of_life.cpp)
setup_example(cassini_oval cassini_oval.cpp)
setup_example(translucent_helix translucent_helix.cpp)
setup_example(surface_mode surface_mode.cpp)
setup_example(gabriels_horn gabriels_horn.cpp)
setup_example(circles circles.cpp)

if(BUILD_SHARED_LIBS)
    # By default the library search path for the executable is set
    # by absolute path. This makes sure to set library search path
    # relative to the executable, and not with absolute directory path.
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH_USE_ORIGIN TRUE)
endif()
