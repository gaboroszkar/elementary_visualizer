#ifndef ELEMENTARY_VISUALIZER_ELEMENTARY_VISUALIZER_HPP
#define ELEMENTARY_VISUALIZER_ELEMENTARY_VISUALIZER_HPP

#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <tl/expected.hpp>

namespace elementary_visualizer
{
template <class T, class E>
using Expected = tl::expected<T, E>;

template <class E>
using Unexpected = tl::unexpected<E>;

class Error
{
public:

    operator std::string() const;
};

class GlTexture;
using RenderedScene = GlTexture;

class Scene
{
public:

    static Expected<Scene, Error> create(
        const glm::ivec2 &size,
        const glm::vec4 &background_color = glm::vec4(1.0f)
    );

    Scene(Scene &&other);
    Scene &operator=(Scene &&other);

    void set_background_color(const glm::vec4 &color);
    Expected<glm::vec4, Error> get_background_color() const;

    std::shared_ptr<const RenderedScene> render();

    ~Scene();

    Scene(const Scene &other) = delete;
    Scene &operator=(const Scene &other) = delete;

private:

    class Impl;
    std::unique_ptr<Impl> impl;

    Scene(std::unique_ptr<Impl> &&impl);
};

enum class RenderMode
{
    fill,
    fit,
    absolute
};

class Window
{
public:

    static Expected<Window, Error> create(
        const std::string &title,
        const glm::ivec2 &size,
        const bool resizable = true
    );

    Window(Window &&other);
    Window &operator=(Window &&other);

    operator bool() const;

    void destroy();
    bool should_close_or_invalid() const;

    void render(
        std::shared_ptr<const RenderedScene> rendered_scene,
        const RenderMode = RenderMode::fill
    );

    ~Window();

    Window(const Window &other) = delete;
    Window &operator=(const Window &other) = delete;

    Window() = delete;

private:

    class Impl;
    std::unique_ptr<Impl> impl;

    Window(std::unique_ptr<Impl> &&impl);
};

void poll_window_events();
}

#endif
