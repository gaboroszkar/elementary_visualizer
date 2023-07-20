function(setup_test TEST_NAME SOURCE_FILE)
    add_executable(${TEST_NAME} ${SOURCE_FILE} test_utilities.cpp)
    set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD 20)
    target_compile_options(${TEST_NAME} PRIVATE -Werror -Wall -Wextra)
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_link_libraries(${TEST_NAME} PRIVATE elementary_visualizer)
    add_test(${TEST_NAME} ${TEST_NAME})
endfunction()

setup_test(simple_scene_test simple_scene_test.cpp)
setup_test(simple_window_test simple_window_test.cpp)
setup_test(window_size_test window_size_test.cpp)
setup_test(window_render_test window_render_test.cpp)
setup_test(window_keyboard_event_test window_keyboard_event_test.cpp)
setup_test(window_mouse_event_test window_mouse_event_test.cpp)
setup_test(linesegments_test linesegments_test.cpp)
setup_test(scene_anti_aliasing_test scene_anti_aliasing_test.cpp)
setup_test(depth_peeling_test depth_peeling_test.cpp)

if(BUILD_SHARED_LIBS)
    # By default the library search path for the executable is set
    # by absolute path. This makes sure to set library search path
    # relative to the executable, and not with absolute directory path.
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH_USE_ORIGIN TRUE)
endif()
