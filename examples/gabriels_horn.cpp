#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

ev::SurfaceData generate_surface_data()
{
    const size_t u_size = 50;
    const size_t v_size = 20;

    ev::SurfaceData surface_data(
        std::vector<ev::Vertex>(u_size * v_size),
        u_size,
        ev::SurfaceData::Mode::smooth
    );

    for (size_t u = 0; u != u_size; ++u)
    {
        for (size_t v = 0; v != v_size; ++v)
        {
            const float fu = static_cast<float>(u) / (u_size - 1);
            const float fv = static_cast<float>(v) / (v_size - 1);

            const float r = 2.0f / (0.2f * v + 1.0f);
            const float phi = fu * 2.0f * std::numbers::pi;

            const float x = r * cosf(fu * phi);
            const float z = r * sinf(fu * phi);
            const float y = 7.0f * (fv - 0.3f);

            surface_data(u, v) = ev::Vertex(
                glm::vec3(x, y, z), glm::vec4(0.0f, 0.75f, 1.0f, 0.8f)
            );
        }
    }

    return surface_data;
}

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 2);
    if (!scene)
        return EXIT_FAILURE;

    auto video =
        ev::Video::create("gabriels_horn.gif", scene_size, 15, 2500000);
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

    auto surface = ev::SurfaceVisual::create(generate_surface_data());
    if (!surface)
        return EXIT_FAILURE;
    surface.value()->set_diffuse_color(glm::vec3(0.5f));
    surface.value()->set_specular_color(glm::vec3(0.8f));
    surface.value()->set_shininess(30.0f);
    scene.value()->add_visual(surface.value());

    const glm::vec3 eye = glm::vec3(0.0f, -10.0f, 2.0f);
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
    while (!window.value()->should_close_or_invalid())
    {
        if (!rotation_delta)
            rotation.x += 0.08f;
        glm::mat4 model = glm::rotate(
            glm::rotate(glm::identity<glm::mat4>(), rotation.y, x_axis),
            rotation.x,
            z_axis
        );
        surface.value()->set_model(model);

        auto rendered_scene = scene.value()->render();
        window.value()->render(rendered_scene);
        video.value()->render(rendered_scene);
        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
