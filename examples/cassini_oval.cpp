#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

// Cassini oval.
// See <https://en.wikipedia.org/wiki/Cassini_oval>,
// <https://mathworld.wolfram.com/CassiniOvals.html>,
// <https://stackoverflow.com/a/70999854>.

float cassini_oval_r(
    const float a, const float b, const float t, const int sign
)
{
    float c = powf(b / a, 4.0f) - powf(sinf(2.0f * t), 2.0f);
    if (c < 0.0f)
        c = 0;
    return a * a * (cosf(2 * t) + sign * sqrtf(c));
}

void add_vertices(
    std::vector<ev::Vertex> &vertices,
    const float a,
    const float b,
    const int sign,
    const float t0,
    const float t1,
    const int x_sign,
    const glm::vec4 &color
)
{
    const size_t n = 100;

    const float f = 1.0f / (n - 1);
    for (size_t i = 0; i != n; ++i)
    {
        const float t = t0 + (i * f) * (t1 - t0);
        const float r = cassini_oval_r(a, b, t, sign);
        const float x = x_sign * r * cosf(t);
        const float y = r * sinf(t);
        vertices.push_back(ev::Vertex(glm::vec3(x, y, 0.0f), color));
    }
}

std::vector<ev::Vertex> generate_lines_data(
    const float a, const float b, const int x_sign, const glm::vec4 &color
)
{
    std::vector<ev::Vertex> vertices;

    if (a < b)
    {
        add_vertices(
            vertices, a, b, 1, 0.0f, 2.0f * std::numbers::pi, 1, color
        );
    }
    else
    {
        const float t = 0.5f * asinf(powf(b / a, 2.0f));

        add_vertices(vertices, a, b, -1, t, -t, x_sign, color);
        add_vertices(vertices, a, b, 1, -t, t, x_sign, color);
    }

    return vertices;
}

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 1);
    if (!scene)
        return EXIT_FAILURE;

    auto video = ev::Video::create("cassini_oval.gif", scene_size, 15, 2500000);
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

    std::vector<std::array<std::shared_ptr<ev::LinesVisual>, 2>> all_lines;
    for (size_t i = 0; i != 12; ++i)
    {
        std::array<std::shared_ptr<ev::LinesVisual>, 2> current_lines;
        for (size_t j = 0; j != 2; ++j)
        {
            auto lines =
                ev::LinesVisual::create(std::vector<ev::Vertex>(), 1.5f);
            if (!lines)
                return EXIT_FAILURE;
            scene.value()->add_visual(lines.value());
            lines.value()->set_view(glm::mat4(1.0f));
            glm::mat4 projection = glm::ortho(-9.0f, 9.0f, -6.0f, 6.0f);
            lines.value()->set_projection(projection);

            current_lines[j] = lines.value();
        }

        all_lines.push_back(current_lines);
    }

    const float a = 2.0f;
    const float min_b = 0.25f;
    const float max_b = 3.5f;

    float t = 0.0f;
    while (!window.value()->should_close_or_invalid())
    {
        for (size_t i = 0; i != all_lines.size(); ++i)
        {
            const float f =
                fmod(t + static_cast<float>(i) / (all_lines.size()), 1.0f);
            const float b = min_b + (f * (max_b - min_b));
            const glm::vec4 color(1.0f - f, 0.0f, 0.0f, 1.0f - f);

            // If a < b, then we can parametrically draw the whole
            // oval with one line, otherwise we need two separate lines.
            if (a < b)
            {
                all_lines[i][0]->set_lines_data(
                    generate_lines_data(a, b, 1, color)
                );
                all_lines[i][1]->set_lines_data(std::vector<ev::Vertex>());
            }
            else
            {
                all_lines[i][0]->set_lines_data(
                    generate_lines_data(a, b, 1, color)
                );
                all_lines[i][1]->set_lines_data(
                    generate_lines_data(a, b, -1, color)
                );
            }
        }

        auto rendered_scene = scene.value()->render();
        video.value()->render(rendered_scene);
        window.value()->render(rendered_scene);
        ev::poll_window_events();

        t += 0.005f;
    }

    return EXIT_SUCCESS;
}
