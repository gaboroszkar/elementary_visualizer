#ifndef ELEMENTARY_VISUALIZER_WINDOW_HPP
#define ELEMENTARY_VISUALIZER_WINDOW_HPP

#include <elementary_visualizer/elementary_visualizer.hpp>
#include <entity.hpp>
#include <gl_resources.hpp>
#include <glfw_resources.hpp>
#include <memory>
#include <optional>

namespace elementary_visualizer
{
class Window::Impl
{
public:

    Impl(
        std::shared_ptr<Entity> entity,
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        std::shared_ptr<GlQuad> quad
    );

    Impl(Impl &&other);
    Impl &operator=(Impl &&other);

    operator bool() const;

    bool should_close_or_invalid() const;

    void render(
        std::shared_ptr<const GlTexture> rendered_scene,
        const RenderMode render_mode
    );

    void destroy();

    ~Impl();

    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

private:

    std::shared_ptr<Entity> entity;
    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    std::shared_ptr<GlQuad> quad;
};
}

#endif
