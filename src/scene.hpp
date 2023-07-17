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
        const glm::vec4 &background_color
    );

    Impl(Impl &&other);
    Impl &operator=(Impl &&other);

    std::shared_ptr<const GlTexture> render();

    ~Impl();

    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

private:

    std::shared_ptr<Entity> entity;
    std::shared_ptr<GlFramebufferTexture> framebuffer_texture;

public:

    glm::vec4 background_color;
};
}

#endif
