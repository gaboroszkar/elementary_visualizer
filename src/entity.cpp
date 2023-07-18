#include <entity.hpp>
#include <shader_sources.hpp>

namespace elementary_visualizer
{
Expected<std::shared_ptr<Entity>, Error> Entity::ensure_initialized_and_get()
{
    static std::optional<Expected<std::shared_ptr<Entity>, Error>> entity =
        std::nullopt;
    if (!entity)
        entity = Entity::initialize();
    return entity.value();
}

Expected<std::shared_ptr<WrappedGlfwWindow>, Error> Entity::create_window(
    const std::string &title, const glm::ivec2 &size, const bool resizable
)
{
    return WrappedGlfwWindow::create(title, size, resizable, this->glfw_window);
}

Expected<std::shared_ptr<GlTexture>, Error> Entity::create_texture(
    const glm::ivec2 &size, const bool depth, const std::optional<int> samples
)
{
    return GlTexture::create(this->glfw_window, size, depth, samples);
}

Expected<std::shared_ptr<GlFramebufferTexture>, Error>
    Entity::create_framebuffer_texture(
        const glm::ivec2 &size, const std::optional<int> samples
    )
{
    return GlFramebufferTexture::create(this->glfw_window, size, samples);
}

Expected<std::shared_ptr<GlLinesegments>, Error>
    Entity::create_linesegments(const std::vector<Linesegment> &linesegments)
{
    return GlLinesegments::create(this->glfw_window, linesegments);
}

void Entity::make_current_context()
{
    this->glfw_window->make_current_context();
}

Entity::~Entity() {}

Expected<std::shared_ptr<Entity>, Error> Entity::initialize()
{
    Expected<std::shared_ptr<WrappedGlfwWindow>, Error> window_creation_result =
        WrappedGlfwWindow::create(
            "Entity", glm::ivec2(1, 1), false, nullptr, false
        );
    return window_creation_result.and_then(
        [](std::shared_ptr<WrappedGlfwWindow> glfw_window
        ) -> Expected<std::shared_ptr<Entity>, Error>
        {
            std::vector<GlShaderSource> quad_shader_sources;
            quad_shader_sources.push_back(quad_vertex_shader_source());
            quad_shader_sources.push_back(quad_fragment_shader_source());
            Expected<std::shared_ptr<GlShaderProgram>, Error>
                quad_shader_program(
                    GlShaderProgram::create(glfw_window, quad_shader_sources)
                );
            if (!quad_shader_program)
                return Unexpected<Error>(Error());

            std::vector<GlShaderSource> linesegments_shader_sources;
            linesegments_shader_sources.push_back(
                linesegments_vertex_shader_source()
            );
            linesegments_shader_sources.push_back(
                linesegments_geometry_shader_source()
            );
            linesegments_shader_sources.push_back(
                linesegments_fragment_shader_source()
            );

            Expected<std::shared_ptr<GlShaderProgram>, Error>
                linesegments_shader_program(GlShaderProgram::create(
                    glfw_window, linesegments_shader_sources
                ));
            if (!linesegments_shader_program)
                return Unexpected<Error>(Error());

            return std::shared_ptr<Entity>(new Entity(
                glfw_window,
                quad_shader_program.value(),
                linesegments_shader_program.value()
            ));
        }
    );
}

Entity::Entity(
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    std::shared_ptr<GlShaderProgram> quad_shader_program,
    std::shared_ptr<GlShaderProgram> linesegments_shader_program
)
    : glfw_window(glfw_window),
      quad_shader_program(quad_shader_program),
      linesegments_shader_program(linesegments_shader_program)
{}
}
