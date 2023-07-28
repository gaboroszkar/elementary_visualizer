#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 1);
    if (!scene)
        return EXIT_FAILURE;

    auto video = ev::Video::create("circles.gif", scene_size, 15, 2500000);
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

    const unsigned int number_of_circles = 10;

    std::vector<std::shared_ptr<ev::CircleVisual>> circles;
    for (unsigned int i = 0; i != number_of_circles; ++i)
    {
        const float phi =
            1.0f * std::numbers::pi * static_cast<float>(i) / number_of_circles;
        const glm::vec2 direction(cosf(phi), sinf(phi));

        glm::vec3 color(0.8f, 0.8f, 0.8f);

        auto lines = ev::LinesVisual::create(
            std::vector<ev::Vertex>({
                ev::Vertex(glm::vec3(-direction, 0.1f), glm::vec4(color, 1.0f)),
                ev::Vertex(glm::vec3(0.0f, 0.0f, 0.1f), glm::vec4(color, 0.0f)),
                ev::Vertex(glm::vec3(direction, 0.1f), glm::vec4(color, 1.0f)),
            }),
            1.0f
        );
        if (!lines)
            return EXIT_FAILURE;
        scene.value()->add_visual(lines.value());

        const float r = 0.0f;
        const float g = 0.5f * (1.0f * sinf(phi));
        const float b = 1.0f - 0.5f * (1.0f * cosf(phi));

        auto circle = ev::CircleVisual::create(glm::vec4(r, g, b, 1.0f));
        if (!circle)
            return EXIT_FAILURE;
        scene.value()->add_visual(circle.value());

        circle.value()->set_view(glm::mat4(1.0f));
        glm::mat4 projection = glm::ortho(-1.0f, +1.0f, -1.0f, +1.0f);
        circle.value()->set_projection(projection);

        circles.push_back(circle.value());
    }

    float t = 0.0f;
    while (!window.value()->should_close_or_invalid())
    {
        for (unsigned int i = 0; i != circles.size(); ++i)
        {
            const float phi = 1.0f * std::numbers::pi * static_cast<float>(i) /
                              circles.size();
            const float magnitude = 0.8f * cosf(phi + t);
            const glm::vec3 direction(cosf(phi), sinf(phi), 0.0f);
            const glm::mat4 model_0 =
                glm::translate(glm::mat4(1.0f), magnitude * direction);
            const glm::mat4 model_1 = glm::scale(model_0, glm::vec3(0.1f));
            circles[i]->set_model(model_1);
        }

        auto rendered_scene = scene.value()->render();
        window.value()->render(rendered_scene);
        video.value()->render(rendered_scene);

        ev::poll_window_events();

        t += 0.05f;
    }

    return EXIT_SUCCESS;
}
