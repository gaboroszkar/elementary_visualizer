#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <gl_resources.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <test_utilities.hpp>

namespace ev = elementary_visualizer;

int main(int, char **)
{
    const glm::uvec2 scene_size(1280, 720);
    auto scene = ev::Scene::create(
        scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), std::nullopt
    );
    if (!scene)
        return EXIT_FAILURE;

    std::vector<ev::Linesegment> linesegments_data;
    ev::Vertex start;
    ev::Vertex end;

    start = ev::Vertex(
        glm::vec3(0.0f, -2.0f, -0.5f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    );
    end = ev::Vertex(
        glm::vec3(0.0f, 2.0f, -0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
    );
    linesegments_data.push_back(ev::Linesegment(start, end, 20.0f));

    start = ev::Vertex(
        glm::vec3(-1.5f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)
    );
    end = ev::Vertex(
        glm::vec3(1.5f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
    );
    linesegments_data.push_back(ev::Linesegment(start, end, 10.0f));

    auto linesegments = ev::LinesegmentsVisual::create(linesegments_data);
    if (!linesegments)
        return EXIT_FAILURE;

    {
        glm::mat4 model(1.0f);
        model = glm::rotate(
            model,
            static_cast<float>(std::numbers::pi) / 4.0f,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );
        linesegments.value()->set_model(model);

        glm::vec3 eye(3.0f, 0.0f, 3.0f);
        glm::vec3 center(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0f, 0.0f, 1.0f);
        glm::mat4 view = glm::lookAt(eye, center, up);
        linesegments.value()->set_view(view);

        float fov = 45.0f;
        float near = 0.01f;
        float far = 100.0f;
        glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
        linesegments.value()->set_projection(projection);
    }

    scene.value()->add_visual(linesegments.value());

    std::shared_ptr<const ev::GlTexture> rendered_scene;

    rendered_scene = scene.value()->render();

    if (rendered_scene_hash(rendered_scene, scene_size) != 2762834654632811456U)
        return EXIT_FAILURE;

    start = ev::Vertex(
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    end = ev::Vertex(
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    linesegments_data.push_back(ev::Linesegment(start, end, 50.0f));

    linesegments.value()->set_linesegments_data(linesegments_data);

    {
        glm::mat4 model(1.0f);
        linesegments.value()->set_model(model);

        glm::vec3 eye(0.0f, -2.0f, 2.0f);
        glm::vec3 center(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0f, 0.0f, 1.0f);
        glm::mat4 view = glm::lookAt(eye, center, up);
        linesegments.value()->set_view(view);

        float fov = 90.0f;
        float near = 0.01f;
        float far = 100.0f;
        glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
        linesegments.value()->set_projection(projection);
    }

    rendered_scene = scene.value()->render();

    if (rendered_scene_hash(rendered_scene, scene_size) != 8076035020585418323U)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
