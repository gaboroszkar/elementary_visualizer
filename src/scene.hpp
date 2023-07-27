#ifndef ELEMENTARY_VISUALIZER_SCENE_HPP
#define ELEMENTARY_VISUALIZER_SCENE_HPP

#include <array>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <entity.hpp>
#include <gl_resources.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <set>

namespace elementary_visualizer
{
struct DepthPeelingData
{
    bool first_pass;
    std::shared_ptr<GlTexture> depth_texture;
    DepthPeelingData(bool first_pass, std::shared_ptr<GlTexture> depth_texture)
        : first_pass(first_pass), depth_texture(depth_texture)
    {}
};

void depth_peeling_set_uniforms(
    std::shared_ptr<GlShaderProgram> shader_program,
    const DepthPeelingData &depth_peeling_data
);

class Scene::Impl
{
public:

    Impl(
        std::shared_ptr<Entity> entity,
        std::shared_ptr<GlFramebufferTexture> framebuffer_texture,
        std::shared_ptr<GlFramebufferTexture>
            framebuffer_texture_possibly_multisampled,
        std::array<std::shared_ptr<GlTexture>, 2> depth_textures,
        std::vector<std::shared_ptr<GlFramebufferTexture>>
            depth_peeling_render_textures,
        const glm::vec4 &background_color
    );

    void add_visual(std::shared_ptr<Visual> visual);
    void remove_visual(std::shared_ptr<Visual> visual);

    std::shared_ptr<const GlTexture> render();

    ~Impl();

    Impl(Impl &&other) = delete;
    Impl &operator=(Impl &&other) = delete;
    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

private:

    std::shared_ptr<Entity> entity;
    std::shared_ptr<GlFramebufferTexture> framebuffer_texture;
    std::shared_ptr<GlFramebufferTexture>
        framebuffer_texture_possibly_multisampled;
    std::array<std::shared_ptr<GlTexture>, 2> depth_textures;
    std::vector<std::shared_ptr<GlFramebufferTexture>>
        depth_peeling_render_textures;
    std::set<std::shared_ptr<Visual>> visuals;

public:

    glm::vec4 background_color;
};
}

#endif
