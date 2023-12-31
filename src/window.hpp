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

    operator bool() const;

    bool should_close_or_invalid() const;

    void render(
        std::shared_ptr<const GlTexture> rendered_scene,
        const RenderMode render_mode
    );

    void destroy();

    void on_keyboard_event(
        std::optional<std::function<void(EventAction, Key, ModifierKey)>>
            function
    );
    void on_mouse_button_event(
        std::optional<
            std::function<void(EventAction, MouseButton, ModifierKey)>> function
    );
    void on_mouse_move_event(
        std::optional<std::function<void(glm::vec2)>> function
    );

    glm::uvec2 get_size() const;

    ~Impl();

    Impl(Impl &&other) = delete;
    Impl &operator=(Impl &&other) = delete;
    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

private:

    std::shared_ptr<Entity> entity;
    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    std::shared_ptr<GlQuad> quad;
};
}

#endif
