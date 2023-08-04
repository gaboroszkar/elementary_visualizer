#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

std::vector<ev::Vertex>
    generate_surface_data(const size_t width_half, const float t)
{
    const int width = (width_half * 2 + 1);
    std::vector<ev::Vertex> vertices(width * width);
    for (int y = -width_half; y <= static_cast<int>(width_half); ++y)
    {
        for (int x = -width_half; x <= static_cast<int>(width_half); ++x)
        {
            float fx = static_cast<float>(x) / width_half;
            float fy = static_cast<float>(y) / width_half;
            float r_squared = ((fx * fx) + (fy * fy));
            float fz = cosf(t) * expf(-1.5 * r_squared) * sinf(2.0f * fx);

            float r = 1.0f * (1.0f - (fz + 0.5f));
            float g = (fz + 0.5f) * 0.25f;
            float b = 1.0f * ((fz + 0.5f));

            const size_t u = x + width_half;
            const size_t v = y + width_half;
            vertices[v * width + u] =
                ev::Vertex(glm::vec3(fx, fy, fz), glm::vec4(r, g, b, 1.0f));
        }
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

    auto video = ev::Video::create("surface_mode.gif", scene_size, 15, 2500000);
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
    glm::vec2 rotation(-0.25f, 0.0f);
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

    const int width_half = 8;
    const size_t width = (width_half * 2 + 1);

    auto surface =
        ev::SurfaceVisual::create(ev::SurfaceData(std::vector<ev::Vertex>(), 0)
        );
    if (!surface)
        return EXIT_FAILURE;
    surface.value()->set_diffuse_color(glm::vec3(0.75f));
    surface.value()->set_specular_color(glm::vec3(1.0f));
    surface.value()->set_shininess(40.0f);
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

        const ev::SurfaceData::Mode mode =
            (fmod(t, 4.0f * std::numbers::pi) < 2.0f * std::numbers::pi)
                ? ev::SurfaceData::Mode::flat
                : ev::SurfaceData::Mode::smooth;
        ev::SurfaceData surface_data(
            generate_surface_data(width_half, t), width, mode
        );
        surface.value()->set_surface_data(surface_data);

        t += 0.1f;

        auto rendered_scene = scene.value()->render();
        window.value()->render(rendered_scene);
        video.value()->render(rendered_scene);
        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
