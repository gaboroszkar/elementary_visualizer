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
    const std::string &title, const glm::uvec2 &size, const bool resizable
)
{
    return WrappedGlfwWindow::create(title, size, resizable, this->glfw_window);
}

Expected<std::shared_ptr<GlTexture>, Error> Entity::create_texture(
    const glm::uvec2 &size, const bool depth, const std::optional<int> samples
)
{
    return GlTexture::create(this->glfw_window, size, depth, samples);
}

Expected<std::shared_ptr<GlFramebufferTexture>, Error>
    Entity::create_framebuffer_texture(
        const glm::uvec2 &size, const std::optional<int> samples
    )
{
    return GlFramebufferTexture::create(this->glfw_window, size, samples);
}

Expected<std::shared_ptr<GlLinesegments>, Error> Entity::create_linesegments(
    const std::vector<Linesegment> &linesegments_data
)
{
    return GlLinesegments::create(this->glfw_window, linesegments_data);
}

Expected<std::shared_ptr<GlLines>, Error>
    Entity::create_lines(const std::vector<Vertex> &lines_data)
{
    return GlLines::create(this->glfw_window, lines_data);
}

Expected<std::shared_ptr<GlSurface>, Error>
    Entity::create_surface(const SurfaceData &surface_data)
{
    return GlSurface::create(this->glfw_window, surface_data);
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
            "Entity", glm::uvec2(1, 1), false, nullptr, false
        );
    return window_creation_result.and_then(
        [](std::shared_ptr<WrappedGlfwWindow> glfw_window
        ) -> Expected<std::shared_ptr<Entity>, Error>
        {
            Expected<std::shared_ptr<GlQuad>, Error> quad(
                GlQuad::create(glfw_window)
            );
            if (!quad)
                return Unexpected<Error>(Error());

            std::vector<GlShaderSource> quad_shader_sources;
            quad_shader_sources.push_back(quad_vertex_shader_source());
            quad_shader_sources.push_back(quad_fragment_shader_source());
            Expected<std::shared_ptr<GlShaderProgram>, Error>
                quad_shader_program(
                    GlShaderProgram::create(glfw_window, quad_shader_sources)
                );
            if (!quad_shader_program)
                return Unexpected<Error>(Error());

            std::vector<GlShaderSource> quad_multisampled_shader_sources;
            quad_multisampled_shader_sources.push_back(
                quad_vertex_shader_source()
            );
            quad_multisampled_shader_sources.push_back(
                quad_multisampled_fragment_shader_source()
            );
            Expected<std::shared_ptr<GlShaderProgram>, Error>
                quad_multisampled_shader_program(GlShaderProgram::create(
                    glfw_window, quad_multisampled_shader_sources
                ));
            if (!quad_multisampled_shader_program)
                return Unexpected<Error>(Error());

            std::vector<GlShaderSource> linesegments_shader_sources;
            linesegments_shader_sources.push_back(
                depth_peeling_fragment_shader_source()
            );
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

            std::vector<GlShaderSource> lines_shader_sources;
            lines_shader_sources.push_back(depth_peeling_fragment_shader_source(
            ));
            lines_shader_sources.push_back(lines_vertex_shader_source());
            lines_shader_sources.push_back(lines_geometry_shader_source());
            lines_shader_sources.push_back(lines_fragment_shader_source());
            Expected<std::shared_ptr<GlShaderProgram>, Error>
                lines_shader_program(
                    GlShaderProgram::create(glfw_window, lines_shader_sources)
                );
            if (!lines_shader_program)
                return Unexpected<Error>(Error());

            std::vector<GlShaderSource> surface_shader_sources;
            surface_shader_sources.push_back(
                depth_peeling_fragment_shader_source()
            );
            surface_shader_sources.push_back(surface_vertex_shader_source());
            surface_shader_sources.push_back(surface_fragment_shader_source());
            Expected<std::shared_ptr<GlShaderProgram>, Error>
                surface_shader_program(
                    GlShaderProgram::create(glfw_window, surface_shader_sources)
                );
            if (!surface_shader_program)
                return Unexpected<Error>(Error());

            return std::shared_ptr<Entity>(new Entity(
                glfw_window,
                quad.value(),
                quad_shader_program.value(),
                quad_multisampled_shader_program.value(),
                linesegments_shader_program.value(),
                lines_shader_program.value(),
                surface_shader_program.value()
            ));
        }
    );
}

Entity::Entity(
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    std::shared_ptr<GlQuad> quad,
    std::shared_ptr<GlShaderProgram> quad_shader_program,
    std::shared_ptr<GlShaderProgram> quad_multisampled_shader_program,
    std::shared_ptr<GlShaderProgram> linesegments_shader_program,
    std::shared_ptr<GlShaderProgram> lines_shader_program,
    std::shared_ptr<GlShaderProgram> surface_shader_program
)
    : glfw_window(glfw_window),
      quad(quad),
      quad_shader_program(quad_shader_program),
      quad_multisampled_shader_program(quad_multisampled_shader_program),
      linesegments_shader_program(linesegments_shader_program),
      lines_shader_program(lines_shader_program),
      surface_shader_program(surface_shader_program)
{}
}
