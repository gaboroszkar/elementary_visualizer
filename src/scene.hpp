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
class Scene::Impl
{
public:

    Impl(
        std::shared_ptr<Entity> entity,
        std::shared_ptr<GlFramebufferTexture> framebuffer_texture,
        std::shared_ptr<GlFramebufferTexture>
            framebuffer_texture_possibly_multisampled,
        std::array<std::shared_ptr<GlTexture>, 1> depth_textures,
        const glm::vec4 &background_color
    );

    Impl(Impl &&other);
    Impl &operator=(Impl &&other);

    void add_visual(std::shared_ptr<Visual> visual);
    void remove_visual(std::shared_ptr<Visual> visual);

    std::shared_ptr<const GlTexture> render();

    ~Impl();

    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

private:

    std::shared_ptr<Entity> entity;
    std::shared_ptr<GlFramebufferTexture> framebuffer_texture;
    std::shared_ptr<GlFramebufferTexture>
        framebuffer_texture_possibly_multisampled;
    std::array<std::shared_ptr<GlTexture>, 1> depth_textures;
    std::set<std::shared_ptr<Visual>> visuals;

public:

    glm::vec4 background_color;
};
}

#endif
