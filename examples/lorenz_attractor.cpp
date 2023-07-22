#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

glm::vec3 lorenz_step(glm::vec3 l)
{
    float dt = 0.01f;
    float sigma = 10.0f;
    float rho = 28.0f;
    float beta = 2.66f;

    float dx = sigma * (l.y - l.x);
    float dy = l.x * (rho - l.z) - l.y;
    float dz = l.x * l.y - beta * l.z;
    l.x += dt * dx;
    l.y += dt * dy;
    l.z += dt * dz;

    return l;
}

int main(int, char **)
{
    const glm::ivec2 scene_size(1280, 720);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 2);
    if (!scene)
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
                    rotation_delta = rotation - mouse_to_rotation * mouse_position;
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
                rotation = mouse_to_rotation * mouse_position + rotation_delta.value();
        }
    );

    std::vector<ev::Vertex> lines_data;
    auto lines = ev::LinesVisual::create(lines_data, 2.0f);
    if (!lines)
        return EXIT_FAILURE;
    scene->add_visual(lines.value());

    glm::vec3 lorenz_point(1.0f, 1.0f, 1.0f);

    glm::vec3 z_axis(0.0f, 0.0f, 1.0f);
    glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
    while (!window.value().should_close_or_invalid())
    {
        const int lorenz_steps_per_frame = 5;
        for (int i = 0; i < lorenz_steps_per_frame; ++i)
        {
            lorenz_point = lorenz_step(lorenz_point);
            lines_data.push_back(ev::Vertex(
                lorenz_point - glm::vec3(0.0f, 0.0f, 30.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
            ));
            if (lines_data.size() > 2000)
                lines_data.erase(lines_data.begin());
            for (unsigned int j = 0; j < lines_data.size(); ++j)
            {
                float f = 1.0f / lines_data.size();
                lines_data[j].color.r = 1.0f - (f * j);
                lines_data[j].color.g = 0.0f;
                lines_data[j].color.b = 0.5f + 0.5f * (f * j);
                lines_data[j].color.a = (f * j);
            }
        }

        lines.value()->set_lines_data(lines_data);

        if (!rotation_delta)
            rotation.x += 0.01f;
        glm::mat4 model = glm::rotate(
            glm::rotate(glm::identity<glm::mat4>(), rotation.y, x_axis),
            rotation.x,
            z_axis
        );
        lines.value()->set_model(model);

        glm::vec3 eye(0.0f, -50.0f, 50.0f);
        glm::vec3 center(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0f, 0.0f, 1.0f);
        glm::mat4 view = glm::lookAt(eye, center, up);
        lines.value()->set_view(view);

        float fov = 45.0f;
        float near = 0.01f;
        float far = 200.0f;
        glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
        lines.value()->set_projection(projection);

        window->render(scene->render());
        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
