#ifndef ELEMENTARY_VISUALIZER_ELEMENTARY_VISUALIZER_HPP
#define ELEMENTARY_VISUALIZER_ELEMENTARY_VISUALIZER_HPP

#include <elementary_visualizer/event_values.hpp>
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

struct Vertex
{
    glm::vec3 position;
    glm::vec4 color;

    Vertex(
        const glm::vec3 &position = glm::vec3(),
        const glm::vec4 &color = glm::vec4()
    )
        : position(position), color(color)
    {}
};

struct Linesegment
{
    Vertex start, end;
    float width;
    Linesegment(
        const Vertex &start = Vertex(),
        const Vertex &end = Vertex(),
        const float width = 1.0f
    )
        : start(start), end(end), width(width)
    {}
};

class Visual
{
public:

    virtual void render(const glm::ivec2 &scene_size) const = 0;
};

class LinesegmentsVisual : public Visual
{
public:

    static Expected<std::shared_ptr<LinesegmentsVisual>, Error>
        create(const std::vector<Linesegment> &linesegments);

    LinesegmentsVisual(LinesegmentsVisual &&other);
    LinesegmentsVisual &operator=(LinesegmentsVisual &&other);

    LinesegmentsVisual(LinesegmentsVisual &other);
    LinesegmentsVisual &operator=(LinesegmentsVisual &other);

    void render(const glm::ivec2 &scene_size) const;

    void set_model(const glm::mat4 &model);
    void set_view(const glm::mat4 &view);
    void set_projection(const glm::mat4 &projection);
    void set_linesegments_data(const std::vector<Linesegment> &linesegments_data
    );

    ~LinesegmentsVisual();

private:

    class Impl;
    std::unique_ptr<Impl> impl;

    LinesegmentsVisual(std::unique_ptr<Impl> impl);
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

    void add_visual(std::shared_ptr<Visual> visual);
    void remove_visual(std::shared_ptr<Visual> visual);

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

    void on_keyboard_event(
        std::optional<std::function<void(EventAction, Key, ModifierKey)>>
            function = std::nullopt
    );
    void on_mouse_button_event(
        std::optional<
            std::function<void(EventAction, MouseButton, ModifierKey)>>
            function = std::nullopt
    );
    void on_mouse_move_event(
        std::optional<std::function<void(glm::vec2)>> function = std::nullopt
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
