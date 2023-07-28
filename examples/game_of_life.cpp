#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ev = elementary_visualizer;

int count_living_neighbors(
    const glm::uvec2 &uv, const size_t u_size, const std::vector<bool> &state
)
{
    const int isize = static_cast<int>(state.size());
    const int iu_size = static_cast<int>(u_size);
    const glm::ivec2 iuv(uv);
    auto in_bounds = [&](const int du, const int dv) -> bool
    {
        return (
            (iuv.x + du >= 0) && (iuv.x + du < iu_size) &&
            ((iuv.y + dv) * iu_size + (iuv.x + du) >= 0) &&
            ((iuv.y + dv) * iu_size + (iuv.x + du) < isize)
        );
    };

    int living_neighbors = 0;
    for (int du = -1; du <= +1; ++du)
    {
        for (int dv = -1; dv <= +1; ++dv)
        {
            // If du and dv are 0, it's not a neighbor,
            // it's the same cell.
            if (du == 0 && dv == 0)
                continue;

            if (in_bounds(du, dv) && state[(uv.y + dv) * u_size + (uv.x + du)])
                ++living_neighbors;
        }
    }

    return living_neighbors;
}

std::vector<bool>
    update_state(const size_t u_size, const std::vector<bool> &state)
{
    std::vector<bool> new_state = state;
    for (size_t i = 0; i < state.size(); ++i)
    {
        const size_t u = i % u_size;
        const glm::uvec2 uv(u, (i - u) / u_size);

        const int living_neighbors = count_living_neighbors(uv, u_size, state);
        if (state[uv.y * u_size + uv.x])
        {
            if (living_neighbors < 2 || living_neighbors > 3)
                new_state[uv.y * u_size + uv.x] = false;
        }
        else if (living_neighbors == 3)
            new_state[uv.y * u_size + uv.x] = true;
    }

    return new_state;
}

ev::SurfaceData
    state_to_surface_data(const size_t width, const std::vector<bool> &state)
{
    std::vector<ev::Vertex> vertices(state.size());

    for (size_t i = 0; i < state.size(); ++i)
    {
        glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
        if (state[i])
            color = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);

        size_t u = i % width;
        float x = 2.0f * static_cast<float>(u) / (width - 1) - 1.0f;
        float y =
            -2.0f * static_cast<float>((i - u) / width) / (width - 1) + 1.0f;
        glm::vec3 position(x, y, 0.0f);
        vertices[i] = ev::Vertex(position, color);
    }

    return ev::SurfaceData(vertices, width, ev::SurfaceData::Mode::flat);
}

int main(int, char **)
{
    const glm::ivec2 scene_size(500, 500);
    auto scene =
        ev::Scene::create(scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4, 4);
    if (!scene)
        return EXIT_FAILURE;

    auto video = ev::Video::create("game_of_life.gif", scene_size, 15, 2500000);
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

    size_t width = 50;
    std::vector<bool> state(width * width, false);

    for (int i = 0; i < 2; ++i)
    {
        size_t o = i * 25;
        state[(o + 5) * width + 1] = true;
        state[(o + 5) * width + 2] = true;
        state[(o + 6) * width + 1] = true;
        state[(o + 6) * width + 2] = true;

        state[(o + 3) * width + 14] = true;
        state[(o + 3) * width + 13] = true;
        state[(o + 4) * width + 12] = true;
        state[(o + 5) * width + 11] = true;
        state[(o + 6) * width + 11] = true;
        state[(o + 7) * width + 11] = true;
        state[(o + 8) * width + 12] = true;
        state[(o + 9) * width + 13] = true;
        state[(o + 9) * width + 14] = true;

        state[(o + 6) * width + 15] = true;

        state[(o + 4) * width + 16] = true;
        state[(o + 5) * width + 17] = true;
        state[(o + 6) * width + 17] = true;
        state[(o + 7) * width + 17] = true;
        state[(o + 8) * width + 16] = true;
        state[(o + 6) * width + 18] = true;

        state[(o + 3) * width + 21] = true;
        state[(o + 4) * width + 21] = true;
        state[(o + 5) * width + 21] = true;
        state[(o + 3) * width + 22] = true;
        state[(o + 4) * width + 22] = true;
        state[(o + 5) * width + 22] = true;
        state[(o + 2) * width + 23] = true;
        state[(o + 6) * width + 23] = true;

        state[(o + 1) * width + 25] = true;
        state[(o + 2) * width + 25] = true;
        state[(o + 6) * width + 25] = true;
        state[(o + 7) * width + 25] = true;

        state[(o + 3) * width + 35] = true;
        state[(o + 3) * width + 36] = true;
        state[(o + 4) * width + 35] = true;
        state[(o + 4) * width + 36] = true;
    }

    auto surface =
        ev::SurfaceVisual::create(state_to_surface_data(width, state));
    if (!surface)
        return EXIT_FAILURE;
    surface.value()->set_ambient_color(glm::vec3(1.0f));
    surface.value()->set_diffuse_color(glm::vec3(0.0f));
    surface.value()->set_specular_color(glm::vec3(0.0f));
    surface.value()->set_shininess(0.0f);
    scene.value()->add_visual(surface.value());

    surface.value()->set_view(glm::mat4(1.0f));
    glm::mat4 projection = glm::ortho(-1.0f, +1.0f, -1.0f, +1.0f);
    surface.value()->set_projection(projection);

    while (!window.value()->should_close_or_invalid())
    {
        auto rendered_scene = scene.value()->render();
        window.value()->render(rendered_scene);
        video.value()->render(rendered_scene);

        state = update_state(width, state);
        surface.value()->set_surface_data(state_to_surface_data(width, state));

        ev::poll_window_events();
    }

    return EXIT_SUCCESS;
}
