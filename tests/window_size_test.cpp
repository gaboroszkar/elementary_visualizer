#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ev = elementary_visualizer;

int main(int, char **)
{
    const glm::uvec2 scene_size_0(1280, 720);
    auto window_0 = ev::Window::create("Window 0", scene_size_0, false);
    if (!window_0)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window_0 = glfwGetCurrentContext();

    const glm::uvec2 scene_size_1(500, 500);
    auto window_1 = ev::Window::create("Window 1", scene_size_1, true);
    if (!window_1)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window_1 = glfwGetCurrentContext();

    if (scene_size_0 != window_0.value().get_size())
        return EXIT_FAILURE;

    if (scene_size_1 != window_1.value().get_size())
        return EXIT_FAILURE;

    int width, height;

    glfwGetWindowSize(glfw_window_0, &width, &height);
    if (width != 1280)
        return EXIT_FAILURE;
    if (height != 720)
        return EXIT_FAILURE;
    glfwGetWindowSize(glfw_window_1, &width, &height);
    if (width != 500)
        return EXIT_FAILURE;
    if (height != 500)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
