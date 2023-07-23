#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

float z_function(const float x, const float y, const float t)
{
    return 0.1f * (sinf(x * 10 - t) + sinf(y * 10 - t));
}

void update_lines_data(
    const int width,
    const int j,
    std::shared_ptr<ev::LinesVisual> lines,
    const float t,
    bool horizontal
)
{
    std::vector<ev::Vertex> lines_data(width);
    const int width_half = (width - 1) / 2;
    for (int i = -width_half; i <= width_half; ++i)
    {
        float fx = static_cast<float>(i) / width_half;
        float fy = static_cast<float>(j) / width_half;
        if (horizontal)
            std::swap(fx, fy);

        float fz = z_function(fx, fy, t);
        lines_data[i + width_half] = ev::Vertex(
            glm::vec3(fx, fy, fz), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    }
    lines->set_lines_data(lines_data);
}

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 4);
    if (!scene)
        return EXIT_FAILURE;

    auto video = ev::Video::create("wireframe.gif", scene_size, 15, 2500000);
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

    const glm::vec3 eye = glm::vec3(0.0f, -2.0f, 3.0f);
    const glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 up(0.0f, 0.0f, 1.0f);
    const glm::mat4 view = glm::lookAt(eye, center, up);

    const float fov = 45.0f;
    const float near = 0.01f;
    const float far = 200.0f;
    glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);

    const int width_half = 30;
    const int width = (width_half * 2 + 1);

    // Creating all the horizontal and vertical lines.
    std::vector<std::array<std::shared_ptr<ev::LinesVisual>, 2>> all_lines(width
    );
    for (auto &lines_pair : all_lines)
    {
        for (auto &lines : lines_pair)
        {
            lines = ev::LinesVisual::create(std::vector<ev::Vertex>())
                        .value_or(nullptr);
            if (!lines)
                return EXIT_FAILURE;
            scene->add_visual(lines);
            lines->set_view(view);
            lines->set_projection(projection);
        }
    }

    float t = 0.0f;
    glm::vec3 z_axis(0.0f, 0.0f, 1.0f);
    glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
    while (!window.value().should_close_or_invalid())
    {
        if (!rotation_delta)
            rotation.x += 0.02f;
        glm::mat4 model = glm::rotate(
            glm::rotate(glm::identity<glm::mat4>(), rotation.y, x_axis),
            rotation.x,
            z_axis
        );
        for (auto &lines_pair : all_lines)
            for (auto &lines : lines_pair)
                lines->set_model(model);

        for (int i = 0; i < static_cast<int>(all_lines.size()); ++i)
        {
            update_lines_data(width, i - width_half, all_lines[i][0], t, true);
            update_lines_data(width, i - width_half, all_lines[i][1], t, false);
        }
        t += 0.1f;

        auto rendered_scene = scene->render();
        video->render(rendered_scene);
        window->render(rendered_scene);
        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
