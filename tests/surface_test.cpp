#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <test_utilities.hpp>

namespace ev = elementary_visualizer;

int main(int, char **)
{
    const glm::ivec2 scene_size(1280, 720);
    auto scene = ev::Scene::create(
        scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), std::nullopt
    );
    if (!scene)
        return EXIT_FAILURE;

    std::vector<ev::Vertex> vertex_data;
    vertex_data.push_back(ev::Vertex(
        glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+0.0f, 1.0f, 0.1f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+2.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)
    ));

    vertex_data.push_back(ev::Vertex(
        glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+0.0f, 0.0f, 0.5f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+2.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)
    ));

    vertex_data.push_back(ev::Vertex(
        glm::vec3(-2.0f, -1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+0.0f, -1.0f, 0.2f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+1.0f, -1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
    ));
    vertex_data.push_back(ev::Vertex(
        glm::vec3(+2.0f, -1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    ));

    const size_t u_size = 5;

    ev::SurfaceData surface_data_flat(
        vertex_data, u_size, ev::SurfaceMode::flat
    );
    ev::SurfaceData surface_data_smooth(
        vertex_data, u_size, ev::SurfaceMode::smooth
    );

    std::vector<std::pair<ev::SurfaceData, size_t>> surface_datas(
        {{surface_data_flat, 6461520896486878588U},
         {surface_data_smooth, 6128946808746318154U}}
    );

    for (auto i : std::vector<std::pair<size_t, size_t>>({{0, 1}, {1, 0}}))
    {
        auto surface = ev::SurfaceVisual::create(surface_datas[i.first].first);
        if (!surface)
            return EXIT_FAILURE;
        scene.value()->add_visual(surface.value());
        surface.value()->set_ambient_color(glm::vec3(0.2f, 0.3f, 0.4f));
        surface.value()->set_diffuse_color(glm::vec3(1.0f, 0.9f, 0.8f));
        surface.value()->set_specular_color(glm::vec3(0.8f, 0.9f, 1.0f));
        surface.value()->set_shininess(32.0f);
        surface.value()->set_light_position(glm::vec3(3.0f, -3.0f, 3.0f));

        glm::mat4 model(1.0f);
        model = glm::rotate(
            model,
            static_cast<float>(std::numbers::pi) / 2.0f,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );
        surface.value()->set_model(model);

        const glm::vec3 eye = glm::vec3(2.0f, -2.0f, 3.5f);
        const glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
        const glm::vec3 up(0.0f, 0.0f, 1.0f);
        const glm::mat4 view = glm::lookAt(eye, center, up);
        surface.value()->set_view(view);

        const float fov = 45.0f;
        const float near = 0.01f;
        const float far = 200.0f;
        glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
        surface.value()->set_projection(projection);

        std::shared_ptr<const ev::GlTexture> rendered_scene;

        rendered_scene = scene.value()->render();
        if (rendered_scene_hash(rendered_scene, scene_size) !=
            surface_datas[i.first].second)
            return EXIT_FAILURE;

        surface.value()->set_surface_data(surface_datas[i.second].first);

        rendered_scene = scene.value()->render();
        if (rendered_scene_hash(rendered_scene, scene_size) !=
            surface_datas[i.second].second)
            return EXIT_FAILURE;

        scene.value()->remove_visual(surface.value());
    }

    return EXIT_SUCCESS;
}
