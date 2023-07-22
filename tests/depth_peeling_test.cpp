#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <gl_resources.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <test_utilities.hpp>

namespace ev = elementary_visualizer;

bool test_depth_peeling_passes(
    std::size_t expected_scene_hash,
    int depth_peeling_passes,
    std::shared_ptr<ev::LinesegmentsVisual> linesegments
);

int main(int, char **)
{
    std::vector<ev::Linesegment> linesegments_data;
    ev::Vertex start;
    ev::Vertex end;

    const float width = 100.0f;
    const float w = sinf(std::numbers::pi / 3.0f);
    const float h = cosf(std::numbers::pi / 3.0f);

    start =
        ev::Vertex(glm::vec3(w, -h, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.8f));
    end = ev::Vertex(
        glm::vec3(0.0f, 1.0f, -0.01f), glm::vec4(0.0f, 0.0f, 1.0f, 0.8f)
    );
    linesegments_data.push_back(ev::Linesegment(start, end, width));

    start = ev::Vertex(
        glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.6f)
    );
    end = ev::Vertex(
        glm::vec3(-w, -h, -0.01f), glm::vec4(0.0f, 1.0f, 0.0f, 0.6f)
    );
    linesegments_data.push_back(ev::Linesegment(start, end, width));

    start =
        ev::Vertex(glm::vec3(-w, -h, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
    end =
        ev::Vertex(glm::vec3(w, -h, -0.01f), glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
    linesegments_data.push_back(ev::Linesegment(start, end, width));

    start = ev::Vertex(
        glm::vec3(-1.5f * w, -h - 0.20f, -0.02f),
        glm::vec4(0.0f, 1.0f, 1.0f, 0.5f)
    );
    end = ev::Vertex(
        glm::vec3(1.5f * w, -h - 0.20f, -0.02f),
        glm::vec4(0.0f, 1.0f, 1.0f, 0.5f)
    );
    linesegments_data.push_back(ev::Linesegment(start, end, width));

    start = ev::Vertex(
        glm::vec3(-1.5f * w, 1.10f, 0.02f), glm::vec4(1.0f, 1.0f, 0.0f, 0.5f)
    );
    end = ev::Vertex(
        glm::vec3(1.5f * w, 1.10f, 0.02f), glm::vec4(1.0f, 1.0f, 0.0f, 0.5f)
    );
    linesegments_data.push_back(ev::Linesegment(start, end, width));

    start = ev::Vertex(
        glm::vec3(0.0f, 1.4f, 0.03f), glm::vec4(0.8f, 0.8f, 0.8f, 0.5f)
    );
    end = ev::Vertex(
        glm::vec3(0.0f, -h - 0.5f, 0.03f), glm::vec4(0.8f, 0.8f, 0.8f, 0.5f)
    );
    linesegments_data.push_back(ev::Linesegment(start, end, width));

    auto linesegments = ev::LinesegmentsVisual::create(linesegments_data);
    if (!linesegments)
        return EXIT_FAILURE;

    glm::mat4 model(1.0f);
    linesegments.value()->set_model(model);

    glm::vec3 eye(0.0f, 0.0f, 3.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(eye, center, up);
    linesegments.value()->set_view(view);

    float fov = 45.0f;
    float near = 0.01f;
    float far = 100.0f;
    glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
    linesegments.value()->set_projection(projection);

    if (!test_depth_peeling_passes(
            14731946144017259646U, 1, linesegments.value()
        ))
        return EXIT_FAILURE;
    if (!test_depth_peeling_passes(
            12048636990535060854U, 2, linesegments.value()
        ))
        return EXIT_FAILURE;
    if (!test_depth_peeling_passes(
            7544272561649467750U, 3, linesegments.value()
        ))
        return EXIT_FAILURE;
    if (!test_depth_peeling_passes(
            2311889671597108974U, 4, linesegments.value()
        ))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool test_depth_peeling_passes(
    std::size_t expected_scene_hash,
    int depth_peeling_passes,
    std::shared_ptr<ev::LinesegmentsVisual> linesegments
)
{
    const glm::uvec2 scene_size(1280, 720);
    std::vector<float> rendered_scene_data(4 * 1280 * 720);
    auto scene = ev::Scene::create(
        scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2, depth_peeling_passes
    );
    if (!scene)
        return false;

    scene->add_visual(linesegments);

    std::shared_ptr<const ev::GlTexture> rendered_scene =
        scene.value().render();
    if (rendered_scene_hash(rendered_scene, scene_size) != expected_scene_hash)
        return false;

    return true;
}
