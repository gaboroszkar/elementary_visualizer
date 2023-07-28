#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

glm::vec3 lorenz_step(glm::vec3 l)
{
    float dt = 0.005f;
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

void update_lorenz_points(std::vector<ev::Vertex> &lines_data)
{
    const int points_to_add = 80;
    const int max_points = 4000;

    // Add new points.
    for (int i = 0; i < points_to_add; ++i)
    {
        glm::vec3 new_lorenz_point = lorenz_step(lines_data.back().position);
        lines_data.push_back(ev::Vertex(new_lorenz_point, glm::vec4()));
    }

    // Remove some points.
    while (lines_data.size() > max_points)
        lines_data.erase(lines_data.begin());

    // Update color of each point,
    // because new points have been added,
    // and maybe some removed.
    for (unsigned int i = 0; i < lines_data.size(); ++i)
    {
        float t = static_cast<float>(i) / (lines_data.size() - 1);
        lines_data[i].color.r = 1.0f - t;
        lines_data[i].color.g = 0.0f;
        lines_data[i].color.b = 0.5f + 0.5f * t;
        lines_data[i].color.a = t;
    }
}

ev::Vertex square_vertex(glm::vec2 position)
{
    return ev::Vertex(
        glm::vec3(position.x, position.y, 0.0f),
        glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)
    );
}

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 2);
    if (!scene)
        return EXIT_FAILURE;

    auto video =
        ev::Video::create("lorenz_attractor.gif", scene_size, 15, 2500000);
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

    const float mouse_to_rotation = 0.01f;
    std::optional<glm::vec2> rotation_delta;
    glm::vec2 mouse_position;
    glm::vec2 rotation(0.0f, 0.0f);
    window.value()->on_mouse_button_event(
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
    window.value()->on_mouse_move_event(
        [&](const glm::vec2 mouse_position_in)
        {
            mouse_position = mouse_position_in;
            if (rotation_delta)
                rotation =
                    mouse_to_rotation * mouse_position + rotation_delta.value();
        }
    );

    std::vector<ev::Linesegment> square_linesegments_data;
    const float square_length = 30.0f;
    for (int x = -1; x != 3; x += 2)
        square_linesegments_data.push_back(ev::Linesegment(
            square_vertex(glm::vec2(square_length * x, -square_length)),
            square_vertex(glm::vec2(square_length * x, square_length))
        ));
    for (int y = -1; y != 3; y += 2)
        square_linesegments_data.push_back(ev::Linesegment(
            square_vertex(glm::vec2(-square_length, square_length * y)),
            square_vertex(glm::vec2(square_length, square_length * y))
        ));
    auto square = ev::LinesegmentsVisual::create(square_linesegments_data);
    if (!square)
        return EXIT_FAILURE;
    scene.value()->add_visual(square.value());

    std::vector<ev::Vertex> lines_data;
    lines_data.push_back(ev::Vertex(glm::vec3(1.0f, 1.0f, 1.0f)));
    auto lines = ev::LinesVisual::create(lines_data, 2.0f);
    if (!lines)
        return EXIT_FAILURE;
    scene.value()->add_visual(lines.value());

    const glm::vec3 eye = glm::vec3(0.0f, -90.0f, 50.0f);
    const glm::vec3 center = glm::vec3(0.0f, 0.0f, 20.0f);
    const glm::vec3 up(0.0f, 0.0f, 1.0f);
    const glm::mat4 view = glm::lookAt(eye, center, up);
    lines.value()->set_view(view);
    square.value()->set_view(view);

    const float fov = 45.0f;
    const float near = 0.01f;
    const float far = 200.0f;
    glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
    lines.value()->set_projection(projection);
    square.value()->set_projection(projection);

    glm::vec3 z_axis(0.0f, 0.0f, 1.0f);
    glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
    while (!window.value()->should_close_or_invalid())
    {
        update_lorenz_points(lines_data);
        lines.value()->set_lines_data(lines_data);

        if (!rotation_delta)
            rotation.x += 0.02f;
        glm::mat4 model = glm::rotate(
            glm::rotate(glm::identity<glm::mat4>(), rotation.y, x_axis),
            rotation.x,
            z_axis
        );
        lines.value()->set_model(model);
        square.value()->set_model(model);

        auto rendered_scene = scene.value()->render();
        video.value()->render(rendered_scene);
        window.value()->render(rendered_scene);
        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
