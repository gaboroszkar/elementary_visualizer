#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

std::vector<ev::Vertex> generate_lines_data(
    const size_t n, const float radius, const float turn_0, const float turn_1
)
{
    std::vector<ev::Vertex> lines_data;
    for (size_t i = 0; i != n; ++i)
    {
        const float t = static_cast<float>(i) / (n - 1);
        const float phi =
            (turn_0 + t * (turn_1 - turn_0)) * 2.0f * std::numbers::pi;
        lines_data.push_back(ev::Vertex(
            glm::vec3(radius * cosf(phi), radius * sinf(phi), 0.0f),
            glm::vec4((1.0f - t), t, 1.0f, 1.0f)
        ));
    }
    return lines_data;
}

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 5);
    if (!scene)
        return EXIT_FAILURE;

    auto video = ev::Video::create("lines_cap.gif", scene_size, 15, 2500000);
    if (!video)
        return EXIT_FAILURE;

    auto window = ev::Window::create("Window", scene_size, false);
    if (!window)
        return EXIT_FAILURE;

    window.value()->on_keyboard_event(
        [&](const ev::EventAction action,
            const ev::Key key,
            const ev::ModifierKey)
        {
            if (action == ev::EventAction::press && key == ev::Key::q)
                window.value()->destroy();
        }
    );

    std::vector<std::shared_ptr<ev::LinesVisual>> lines;
    for (size_t i = 0; i != 4; ++i)
    {
        auto line = ev::LinesVisual::create(
            std::vector<ev::Vertex>(),
            25.0f,
            (i % 2 == 0) ? ev::LineCap::butt : ev::LineCap::round
        );
        if (!line)
            return EXIT_FAILURE;
        scene.value()->add_visual(line.value());
        line.value()->set_view(glm::mat4(1.0f));
        glm::mat4 projection = glm::ortho(-1.0f, +1.0f, -1.0f, +1.0f);
        line.value()->set_projection(projection);
        line.value()->set_model(glm::mat4(1.0f));

        lines.push_back(line.value());
    }

    float t = 0.0f;
    while (!window.value()->should_close_or_invalid())
    {
        for (size_t i = 0; i != lines.size(); ++i)
        {
            const int sign = (i % 2 == 0) ? -1 : +1;
            const float turn_0 = sign * t * (0.5f - i * 0.1f);
            const float turn_1 = turn_0 + sign * 0.75f;
            lines[i]->set_lines_data(
                generate_lines_data(50, 0.2f + i * 0.15f, turn_0, turn_1)
            );
        }

        t += 0.04f;

        auto rendered_scene = scene.value()->render();
        video.value()->render(rendered_scene);
        window.value()->render(rendered_scene);
        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
