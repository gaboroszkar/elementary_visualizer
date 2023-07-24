#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 5);
    if (!scene)
        return EXIT_FAILURE;

    auto video =
        ev::Video::create("translucent_helix.gif", scene_size, 15, 2500000);
    if (!video)
        return EXIT_FAILURE;

    auto window = ev::Window::create("Window", scene_size, false);
    if (!window)
        return EXIT_FAILURE;

    window->on_keyboard_event(
        [&](const ev::EventAction action,
            const ev::Key key,
            const ev::ModifierKey)
        {
            if (action == ev::EventAction::press && key == ev::Key::q)
                window->destroy();
        }
    );

    const float mouse_to_rotation = 0.01f;
    std::optional<glm::vec2> rotation_delta;
    glm::vec2 mouse_position;
    glm::vec2 rotation(0.0f, 0.0f);
    window->on_mouse_button_event(
        [&](const ev::EventAction action,
            const ev::MouseButton button,
            const ev::ModifierKey)
        {
            if (button == ev::MouseButton::left)
            {
                if (action == ev::EventAction::press)
                    rotation_delta =
                        rotation - mouse_to_rotation * mouse_position;
                else if (action == ev::EventAction::release)
                    rotation_delta = std::nullopt;
            }
        }
    );
    window->on_mouse_move_event(
        [&](const glm::vec2 mouse_position_in)
        {
            mouse_position = mouse_position_in;
            if (rotation_delta)
                rotation =
                    mouse_to_rotation * mouse_position + rotation_delta.value();
        }
    );

    std::vector<ev::Vertex> lines_data;

    const size_t n = 200;
    const float radius = 0.8f;
    const float turns = 5.0f;
    const float height = 1.75f;
    for (size_t i = 0; i != n; ++i)
    {
        const float t = static_cast<float>(i) / (n - 1);
        const float phi = t * turns * 2.0f * std::numbers::pi;
        lines_data.push_back(ev::Vertex(
            glm::vec3(
                radius * cosf(phi), radius * sinf(phi), (t - 0.5f) * height
            ),
            glm::vec4((1.0f - t), 0.0f, t, t)
        ));
    }

    auto lines = ev::LinesVisual::create(lines_data, 40.0f);
    if (!lines)
        return EXIT_FAILURE;
    scene->add_visual(lines.value());

    const glm::vec3 eye = glm::vec3(0.0f, -2.0f, 2.0f);
    const glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 up(0.0f, 0.0f, 1.0f);
    const glm::mat4 view = glm::lookAt(eye, center, up);
    lines.value()->set_view(view);

    const float fov = 45.0f;
    const float near = 0.01f;
    const float far = 200.0f;
    glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
    lines.value()->set_projection(projection);

    glm::vec3 z_axis(0.0f, 0.0f, 1.0f);
    glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
    while (!window.value().should_close_or_invalid())
    {
        if (!rotation_delta)
            rotation.y += 0.05f;
        glm::mat4 model = glm::rotate(
            glm::rotate(glm::identity<glm::mat4>(), rotation.y, x_axis),
            rotation.x,
            z_axis
        );
        lines.value()->set_model(model);

        auto rendered_scene = scene->render();
        video->render(rendered_scene);
        window->render(rendered_scene);
        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
