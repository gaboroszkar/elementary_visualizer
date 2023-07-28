#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

void update_surface_data(ev::SurfaceData &surface_data, const float t)
{
    const int width_half = (surface_data.u_size - 1) / 2;
    for (int y = -width_half; y <= width_half; ++y)
    {
        for (int x = -width_half; x <= width_half; ++x)
        {
            float fx = static_cast<float>(x) / width_half;
            float fy = static_cast<float>(y) / width_half;
            float r_squared = ((fx * fx) + (fy * fy));
            float fz = 0.5f * expf(-1.5 * r_squared) * cosf(15 * r_squared - t);

            const glm::uvec2 uv(x + width_half, y + width_half);
            surface_data(uv) = ev::Vertex(
                glm::vec3(fx, fy, fz), glm::vec4(0.0f, 0.5f, 1.0f, 0.8f)
            );
        }
    }
}

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 4);
    if (!scene)
        return EXIT_FAILURE;

    auto video = ev::Video::create("wave.gif", scene_size, 15, 2500000);
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

    const int width_half = 100;
    const int width = (width_half * 2 + 1);
    std::vector<ev::Vertex> vertices(width * width);
    ev::SurfaceData surface_data(
        vertices, width, ev::SurfaceData::Mode::smooth
    );
    update_surface_data(surface_data, 0.0f);

    auto surface = ev::SurfaceVisual::create(surface_data);
    if (!surface)
        return EXIT_FAILURE;
    scene.value()->add_visual(surface.value());

    const glm::vec3 eye = glm::vec3(0.0f, -2.0f, 2.0f);
    const glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 up(0.0f, 0.0f, 1.0f);
    const glm::mat4 view = glm::lookAt(eye, center, up);
    surface.value()->set_view(view);

    const float fov = 45.0f;
    const float near = 0.01f;
    const float far = 200.0f;
    glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
    surface.value()->set_projection(projection);

    glm::vec3 z_axis(0.0f, 0.0f, 1.0f);
    glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
    float t = 0.0f;
    while (!window.value()->should_close_or_invalid())
    {
        glm::mat4 model = glm::rotate(
            glm::rotate(glm::identity<glm::mat4>(), rotation.y, x_axis),
            rotation.x,
            z_axis
        );
        surface.value()->set_model(model);

        update_surface_data(surface_data, t);
        surface.value()->set_surface_data(surface_data);
        t += 0.2f;

        auto rendered_scene = scene.value()->render();
        window.value()->render(rendered_scene);
        video.value()->render(rendered_scene);
        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
