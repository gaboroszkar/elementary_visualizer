#include <chrono>
#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <thread>

namespace ev = elementary_visualizer;

bool compare_data(
    const std::vector<float> &rendered_scene_data,
    float r,
    float g,
    float b,
    float a
);

int main(int, char **)
{
    const glm::ivec2 scene_size_0(1280, 720);
    std::vector<float> pixel_data_0(4 * 1280 * 720);
    auto window_0 = ev::Window::create("Window 0", scene_size_0, false);
    if (!window_0)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window_0 = glfwGetCurrentContext();

    const glm::ivec2 scene_size_1(500, 500);
    std::vector<float> pixel_data_1(4 * 500 * 500);
    auto window_1 = ev::Window::create("Window 1", scene_size_1, true);
    if (!window_1)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window_1 = glfwGetCurrentContext();

    if (glfwGetWindowAttrib(glfw_window_0, GLFW_VISIBLE) != GLFW_TRUE)
        return EXIT_FAILURE;
    if (glfwGetWindowAttrib(glfw_window_1, GLFW_VISIBLE) != GLFW_TRUE)
        return EXIT_FAILURE;
    if (glfwGetWindowAttrib(glfw_window_0, GLFW_RESIZABLE) != GLFW_FALSE)
        return EXIT_FAILURE;
    if (glfwGetWindowAttrib(glfw_window_1, GLFW_RESIZABLE) != GLFW_TRUE)
        return EXIT_FAILURE;

    if (window_0->should_close_or_invalid())
        return EXIT_FAILURE;
    if (window_1->should_close_or_invalid())
        return EXIT_FAILURE;

    auto scene_0 =
        ev::Scene::create(scene_size_0, glm::vec4(0.25f, 0.5f, 0.75f, 1.0f));
    if (!scene_0)
        return EXIT_FAILURE;
    auto scene_1 =
        ev::Scene::create(scene_size_1, glm::vec4(0.1f, 0.2f, 0.3f, 0.4f));
    if (!scene_1)
        return EXIT_FAILURE;

    scene_0.value().render();
    window_0->render(scene_0.value().render());
    window_1->render(scene_1.value().render());

    // Wait until rendering is done.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    glfwMakeContextCurrent(glfw_window_0);
    glReadPixels(0, 0, 1280, 720, GL_RGBA, GL_FLOAT, &pixel_data_0[0]);
    if (!compare_data(pixel_data_0, 0.25f, 0.5f, 0.75f, 1.0f))
        return EXIT_FAILURE;
    if (compare_data(pixel_data_0, 0.0f, 0.0f, 0.0f, 0.0f))
        return EXIT_FAILURE;

    glfwMakeContextCurrent(glfw_window_1);
    glReadPixels(0, 0, 500, 500, GL_RGBA, GL_FLOAT, &pixel_data_1[0]);
    glReadPixels(0, 0, 500, 500, GL_RGBA, GL_FLOAT, &pixel_data_1[0]);
    if (!compare_data(pixel_data_1, 0.1f, 0.2f, 0.3f, 0.4f))
        return EXIT_FAILURE;
    if (compare_data(pixel_data_1, 0.0f, 0.0f, 0.0f, 0.0f))
        return EXIT_FAILURE;

    if (window_0->should_close_or_invalid())
        return EXIT_FAILURE;
    window_0->destroy();
    if (!window_0->should_close_or_invalid())
        return EXIT_FAILURE;

    if (window_1->should_close_or_invalid())
        return EXIT_FAILURE;
    window_1->destroy();
    if (!window_1->should_close_or_invalid())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool compare_data(
    const std::vector<float> &rendered_scene_data,
    float r,
    float g,
    float b,
    float a
)
{
    const float threshold = 0.0025f;

    for (size_t i = 0; i != rendered_scene_data.size(); ++i)
    {
        switch (i % 4)
        {
        case 0:
            if (std::fabs(rendered_scene_data[i] - r) > threshold)
                return false;
            break;
        case 1:
            if (std::fabs(rendered_scene_data[i] - g) > threshold)
                return false;
            break;
        case 2:
            if (std::fabs(rendered_scene_data[i] - b) > threshold)
                return false;
            break;
        case 3:
            if (std::fabs(rendered_scene_data[i] - a) > threshold)
                return false;
            break;
        }
    }
    return true;
}
