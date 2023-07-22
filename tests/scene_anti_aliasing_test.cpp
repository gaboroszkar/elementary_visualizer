#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <gl_resources.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <test_utilities.hpp>

namespace ev = elementary_visualizer;

bool test_anti_aliasing_level(
    std::size_t expected_scene_hash,
    std::optional<int> samples,
    std::shared_ptr<ev::LinesegmentsVisual> linesegments
);

int main(int, char **)
{
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

    glm::mat4 model(1.0f);
    linesegments.value()->set_model(model);

    glm::vec3 eye(2.0f, 2.0f, 2.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 0.0f, 1.0f);
    glm::mat4 view = glm::lookAt(eye, center, up);
    linesegments.value()->set_view(view);

    float fov = 45.0f;
    float near = 0.01f;
    float far = 100.0f;
    glm::mat4 projection = glm::perspective(fov, 1.0f, near, far);
    linesegments.value()->set_projection(projection);

    if (!test_anti_aliasing_level(
            5737928650510807612U, std::nullopt, linesegments.value()
        ))
        return EXIT_FAILURE;
    if (!test_anti_aliasing_level(
            10357561896456050140U, 1, linesegments.value()
        ))
        return EXIT_FAILURE;
    if (!test_anti_aliasing_level(
            10357561896456050140U, 2, linesegments.value()
        ))
        return EXIT_FAILURE;
    if (!test_anti_aliasing_level(
            14192143915217897372U, 4, linesegments.value()
        ))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool test_anti_aliasing_level(
    std::size_t expected_scene_hash,
    std::optional<int> samples,
    std::shared_ptr<ev::LinesegmentsVisual> linesegments
)
{
    const glm::uvec2 scene_size(1280, 720);
    std::vector<float> rendered_scene_data(4 * 1280 * 720);
    auto scene = ev::Scene::create(
        scene_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), samples
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
