#include <chrono>
#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <gl_resources.hpp>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <thread>

namespace ev = elementary_visualizer;

std::vector<float> generate_data(const glm::uvec2 &size);
std::vector<float> generate_incorrect_data(const glm::uvec2 &size);
bool compare_data(
    const std::vector<float> &window_data,
    const glm::uvec2 &window_size,
    const std::vector<float> &scene_data,
    const glm::uvec2 &scene_size,
    const ev::RenderMode render_mode
);

int main(int, char **)
{
    const glm::uvec2 window_size(1280, 720);
    std::vector<float> window_data(4 * 1280 * 720);
    auto window = ev::Window::create("Window", window_size, false);
    if (!window)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window = glfwGetCurrentContext();

    // We create an alternative shared_ptr to our window.
    // This is needed for us to create GlTexture for
    // the dummy rendered scene.
    // (Note, we do want to create a Scene in this test.)
    // This is dangerous, we need to
    // not allow this pointer to destroy our window,
    // since the original `window` will destroy our window.
    // That's why we need custom deleter here.
    std::shared_ptr<ev::WrappedGlfwWindow> wrapped_glfw_window(
        static_cast<ev::WrappedGlfwWindow *>(
            glfwGetWindowUserPointer(glfw_window)
        ),
        [](auto) {}
    );

    for (auto render_mode :
         {ev::RenderMode::fit, ev::RenderMode::fill, ev::RenderMode::absolute})
    {
        for (auto scene_size :
             {glm::uvec2(100, 200),
              glm::uvec2(400, 150),
              glm::uvec2(2000, 1500)})
        {
            auto rendered_scene = ev::GlTexture::create(
                wrapped_glfw_window, scene_size, false, std::nullopt
            );
            if (!rendered_scene)
                return EXIT_FAILURE;

            // Uploading the dummy rendered scene.
            rendered_scene.value()->bind();
            std::vector<float> scene_data = generate_data(scene_size);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA32F,
                scene_size.x,
                scene_size.y,
                0,
                GL_RGBA,
                GL_FLOAT,
                &scene_data[0]
            );
            window->render(rendered_scene.value(), render_mode);

            // Wait until rendering is done.
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            glfwMakeContextCurrent(glfw_window);
            glReadPixels(0, 0, 1280, 720, GL_RGBA, GL_FLOAT, &window_data[0]);
            if (!compare_data(
                    window_data,
                    window_size,
                    scene_data,
                    scene_size,
                    render_mode
                ))
                return EXIT_FAILURE;

            // Check wheter the previous comparisons worked,
            // by comparing it to an incorrect data,
            // and see if it fails.
            std::vector<float> incorrect_scene_data =
                generate_incorrect_data(scene_size);
            if (compare_data(
                    window_data,
                    window_size,
                    incorrect_scene_data,
                    scene_size,
                    render_mode
                ))
                return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

std::vector<float> generate_data(const glm::uvec2 &size)
{
    std::vector<float> data(4 * size.x * size.y);
    for (unsigned int x = 0; x < size.x; ++x)
    {
        for (unsigned int y = 0; y < size.y; ++y)
        {
            data[4 * (size.x * y + x) + 0] = static_cast<float>(x) / size.x;
            data[4 * (size.x * y + x) + 1] = static_cast<float>(y) / size.y;
            data[4 * (size.x * y + x) + 2] =
                1.0f - static_cast<float>(x) / size.x;
            data[4 * (size.x * y + x) + 3] =
                1.0f - static_cast<float>(y) / size.y;
        }
    }
    return data;
}

std::vector<float> generate_incorrect_data(const glm::uvec2 &size)
{
    std::vector<float> data(4 * size.x * size.y);
    for (unsigned int x = 0; x < size.x; ++x)
        for (unsigned int y = 0; y < size.y; ++y)
            for (unsigned int z = 0; z < 4; ++z)
                data[4 * (size.x * y + x) + z] = 0.0f;
    return data;
}

glm::uvec3 calculate_window_coordinate(
    const unsigned int window_coordinate_index, const glm::uvec2 &window_size
)
{
    unsigned int color = window_coordinate_index % 4;
    unsigned int position = (window_coordinate_index - color) / 4;
    unsigned int window_coordinate_x = position % window_size.x;
    unsigned int window_coordinate_y =
        (position - window_coordinate_x) / window_size.x;
    return glm::uvec3(window_coordinate_x, window_coordinate_y, color);
}

glm::ivec3 calculate_scene_coordinate(
    const glm::uvec3 &window_coordinate,
    const glm::uvec2 &window_size,
    const glm::uvec2 &scene_size,
    const ev::RenderMode render_mode
)
{
    glm::vec2 window_r = glm::vec2(
        static_cast<float>(window_coordinate.x) / window_size.x,
        static_cast<float>(window_coordinate.y) / window_size.y
    );
    float window_aspect = static_cast<float>(window_size.x) / window_size.y;
    glm::ivec3 scene_fill_horizontal(
        roundf(scene_size.x * window_r.x),
        roundf(
            +0.5f * scene_size.y +
            (scene_size.x / window_aspect) * (window_r.y - 0.5f)
        ),
        window_coordinate.z
    );
    glm::ivec3 scene_fill_vertical(
        roundf(
            +0.5f * scene_size.x +
            (scene_size.y * window_aspect) * (window_r.x - 0.5f)
        ),
        roundf(scene_size.y * window_r.y),
        window_coordinate.z
    );

    float scene_aspect = static_cast<float>(scene_size.x) / scene_size.y;

    switch (render_mode)
    {
    case ev::RenderMode::fill:
        if (scene_aspect < window_aspect)
            return scene_fill_horizontal;
        else
            return scene_fill_vertical;
    case ev::RenderMode::fit:
        if (scene_aspect < window_aspect)
            return scene_fill_vertical;
        else
            return scene_fill_horizontal;
    case ev::RenderMode::absolute:
        return glm::ivec3(
            window_coordinate.x + 0.5f * (static_cast<int>(scene_size.x) -
                                          static_cast<int>(window_size.x)),
            window_coordinate.y + 0.5f * (static_cast<int>(scene_size.y) -
                                          static_cast<int>(window_size.y)),
            window_coordinate.z
        );
    }
    return glm::ivec3();
}

unsigned int calculate_scene_coordinate_index(
    const glm::uvec3 &scene_coordinate, const glm::uvec2 &scene_size
)
{
    return 4 * (scene_size.x * scene_coordinate.y + scene_coordinate.x) +
           scene_coordinate.z;
}

bool compare_data(
    const std::vector<float> &window_data,
    const glm::uvec2 &window_size,
    const std::vector<float> &scene_data,
    const glm::uvec2 &scene_size,
    const ev::RenderMode render_mode
)
{
    const float threshold = 0.015f;
    // At the borders of the scene/window there are
    // small artifacts, and comparisons
    // become impossible, and irrelevant.
    const int border = 10;

    for (size_t window_coordinate_index = 0;
         window_coordinate_index != window_data.size();
         ++window_coordinate_index)
    {
        glm::uvec3 window_coordinate(
            calculate_window_coordinate(window_coordinate_index, window_size)
        );
        glm::ivec3 scene_coordinate(calculate_scene_coordinate(
            window_coordinate, window_size, scene_size, render_mode
        ));
        if (window_coordinate.x > border &&
            window_coordinate.x < (window_size.x - border) &&
            window_coordinate.y > border &&
            window_coordinate.y < (window_size.y - border) &&
            scene_coordinate.x > border &&
            scene_coordinate.x < (static_cast<int>(scene_size.x) - border) &&
            scene_coordinate.y > border &&
            scene_coordinate.y < (static_cast<int>(scene_size.y) - border))
        {
            unsigned int scene_coordinate_index(
                calculate_scene_coordinate_index(scene_coordinate, scene_size)
            );
            if (std::fabs(
                    scene_data[scene_coordinate_index] -
                    window_data[window_coordinate_index]
                ) > threshold)
            {
                return false;
            }
        }
    }
    return true;
}
