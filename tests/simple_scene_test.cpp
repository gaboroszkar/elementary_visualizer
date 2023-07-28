#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <gl_resources.hpp>

namespace ev = elementary_visualizer;

bool compare_data(
    const std::vector<float> &rendered_scene_data,
    float r,
    float g,
    float b,
    float a
);

int main(int, char **)
{
    const glm::uvec2 scene_size_0(1280, 720);
    std::vector<float> rendered_scene_data_0(4 * 1280 * 720);
    auto scene_0 = ev::Scene::create(
        scene_size_0, glm::vec4(0.25f, 0.5f, 0.75f, 1.0f), std::nullopt
    );
    if (!scene_0)
        return EXIT_FAILURE;

    const glm::uvec2 scene_size_1(500, 500);
    std::vector<float> rendered_scene_data_1(4 * 500 * 500);
    auto scene_1 = ev::Scene::create(
        scene_size_1, glm::vec4(0.1f, 0.2f, 0.3f, 0.4f), std::nullopt
    );
    if (!scene_1)
        return EXIT_FAILURE;

    if (scene_0.value()->get_background_color() !=
        glm::vec4(0.25f, 0.5f, 0.75f, 1.0f))
        return EXIT_FAILURE;

    if (scene_1.value()->get_background_color() !=
        glm::vec4(0.1f, 0.2f, 0.3f, 0.4f))
        return EXIT_FAILURE;

    std::shared_ptr<const ev::GlTexture> rendered_scene_0 =
        scene_0.value()->render();
    std::shared_ptr<const ev::GlTexture> rendered_scene_1 =
        scene_1.value()->render();

    rendered_scene_0->bind();
    glGetTexImage(
        GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &rendered_scene_data_0[0]
    );
    if (!compare_data(rendered_scene_data_0, 0.25f, 0.5f, 0.75f, 1.0f))
        return EXIT_FAILURE;

    rendered_scene_1->bind();
    glGetTexImage(
        GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &rendered_scene_data_1[0]
    );
    if (!compare_data(rendered_scene_data_1, 0.1f, 0.2f, 0.3f, 0.4f))
        return EXIT_FAILURE;

    scene_0.value()->set_background_color(glm::vec4(1.0f, 0.8f, 0.6f, 0.4f));
    scene_1.value()->set_background_color(glm::vec4(0.9f, 0.7f, 0.5f, 0.3f));

    if (scene_0.value()->get_background_color().value() !=
        glm::vec4(1.0f, 0.8f, 0.6f, 0.4f))
        return EXIT_FAILURE;

    if (scene_1.value()->get_background_color().value() !=
        glm::vec4(0.9f, 0.7f, 0.5f, 0.3f))
        return EXIT_FAILURE;

    rendered_scene_0 = scene_0.value()->render();
    rendered_scene_1 = scene_1.value()->render();

    rendered_scene_0->bind();
    glGetTexImage(
        GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &rendered_scene_data_0[0]
    );
    if (!compare_data(rendered_scene_data_0, 1.0f, 0.8f, 0.6f, 0.4f))
        return EXIT_FAILURE;

    rendered_scene_1->bind();
    glGetTexImage(
        GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &rendered_scene_data_1[0]
    );
    if (!compare_data(rendered_scene_data_1, 0.9f, 0.7f, 0.5f, 0.3f))
        return EXIT_FAILURE;

    // We check whether comparisons were successful
    // by comparing the rendered scene to an incorrect data,
    // and see if the comparons fail.
    rendered_scene_0->bind();
    glGetTexImage(
        GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &rendered_scene_data_0[0]
    );
    if (compare_data(rendered_scene_data_0, 0.0f, 0.0f, 0.0f, 0.0f))
        return EXIT_FAILURE;
    rendered_scene_1->bind();
    glGetTexImage(
        GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &rendered_scene_data_1[0]
    );
    if (compare_data(rendered_scene_data_1, 0.0f, 0.0f, 0.0f, 0.0f))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool compare_data(
    const std::vector<float> &rendered_scene_data,
    float r,
    float g,
    float b,
    float a
)
{
    for (size_t i = 0; i != rendered_scene_data.size(); ++i)
    {
        switch (i % 4)
        {
        case 0:
            if (rendered_scene_data[i] != r)
                return false;
            break;
        case 1:
            if (rendered_scene_data[i] != g)
                return false;
            break;
        case 2:
            if (rendered_scene_data[i] != b)
                return false;
            break;
        case 3:
            if (rendered_scene_data[i] != a)
                return false;
            break;
        }
    }
    return true;
}
