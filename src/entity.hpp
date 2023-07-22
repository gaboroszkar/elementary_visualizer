#ifndef ELEMENTARY_VISUALIZER_ENTITY_HPP
#define ELEMENTARY_VISUALIZER_ENTITY_HPP

#include <elementary_visualizer/elementary_visualizer.hpp>
#include <gl_resources.hpp>
#include <gl_shader_program.hpp>
#include <glfw_resources.hpp>
#include <memory>
#include <optional>
#include <string>

namespace elementary_visualizer
{
class Entity
{
public:

    static Expected<std::shared_ptr<Entity>, Error>
        ensure_initialized_and_get();

    Expected<std::shared_ptr<WrappedGlfwWindow>, Error> create_window(
        const std::string &title, const glm::uvec2 &size, const bool resizable
    );
    Expected<std::shared_ptr<GlTexture>, Error> create_texture(
        const glm::uvec2 &size,
        const bool depth,
        const std::optional<int> samples
    );
    Expected<std::shared_ptr<GlFramebufferTexture>, Error>
        create_framebuffer_texture(
            const glm::uvec2 &size, const std::optional<int> samples
        );
    Expected<std::shared_ptr<GlLinesegments>, Error>
        create_linesegments(const std::vector<Linesegment> &linesegments_data);
    Expected<std::shared_ptr<GlLines>, Error>
        create_lines(const std::vector<Vertex> &lines_data);

    void make_current_context();

    ~Entity();

    Entity(Entity &&other) = delete;
    Entity &operator=(Entity &&other) = delete;
    Entity(const Entity &other) = delete;
    Entity &operator=(const Entity &other) = delete;

private:

    static Expected<std::shared_ptr<Entity>, Error> initialize();

    Entity(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        std::shared_ptr<GlQuad> quad,
        std::shared_ptr<GlShaderProgram> quad_shader_program,
        std::shared_ptr<GlShaderProgram> quad_multisampled_shader_program,
        std::shared_ptr<GlShaderProgram> linesegments_shader_program,
        std::shared_ptr<GlShaderProgram> lines_shader_program
    );

    std::shared_ptr<WrappedGlfwWindow> glfw_window;

public:

    const std::shared_ptr<GlQuad> quad;
    const std::shared_ptr<GlShaderProgram> quad_shader_program;
    const std::shared_ptr<GlShaderProgram> quad_multisampled_shader_program;
    const std::shared_ptr<GlShaderProgram> linesegments_shader_program;
    const std::shared_ptr<GlShaderProgram> lines_shader_program;
};
}

#endif
