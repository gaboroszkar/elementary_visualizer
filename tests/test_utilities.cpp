#include <iostream>
#include <test_utilities.hpp>

namespace ev = elementary_visualizer;

std::size_t rendered_scene_hash(
    const std::shared_ptr<const ev::GlTexture> &rendered_scene,
    const glm::ivec2 &size,
    const bool debug
)
{
    std::vector<float> rendered_scene_data(4 * size.x * size.y);
    rendered_scene->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &rendered_scene_data[0]);

    std::size_t seed = 0;

    std::hash<float> hasher;
    for (const auto &value : rendered_scene_data)
        seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    if (debug)
    {
        std::cout << seed << "U" << std::endl;

        auto window = ev::Window::create("Window", size, false);
        if (!window)
            return EXIT_FAILURE;
        window.value().render(rendered_scene);
        while (!window.value().should_close_or_invalid())
            ev::poll_window_events();
    }

    return seed;
}
