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

struct DepthPeelingData;

class Visual
{
public:

    virtual void render(
        const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
    ) const = 0;
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

class LinesegmentsVisual : public Visual
{
public:

    static Expected<std::shared_ptr<LinesegmentsVisual>, Error>
        create(const std::vector<Linesegment> &linesegments_data);

    LinesegmentsVisual(LinesegmentsVisual &&other);
    LinesegmentsVisual &operator=(LinesegmentsVisual &&other);

    LinesegmentsVisual(LinesegmentsVisual &other);
    LinesegmentsVisual &operator=(LinesegmentsVisual &other);

    void render(
        const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
    ) const;

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

class LinesVisual : public Visual
{
public:

    static Expected<std::shared_ptr<LinesVisual>, Error>
        create(const std::vector<Vertex> &lines_data, const float width = 1.0f);

    LinesVisual(LinesVisual &&other);
    LinesVisual &operator=(LinesVisual &&other);

    LinesVisual(LinesVisual &other);
    LinesVisual &operator=(LinesVisual &other);

    void render(
        const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
    ) const;

    void set_model(const glm::mat4 &model);
    void set_view(const glm::mat4 &view);
    void set_projection(const glm::mat4 &projection);
    void set_lines_data(const std::vector<Vertex> &lines_data);
    void set_width(const float width);

    ~LinesVisual();

private:

    class Impl;
    std::unique_ptr<Impl> impl;

    LinesVisual(std::unique_ptr<Impl> impl);
};

class GlTexture;
using RenderedScene = GlTexture;

class Scene
{
public:

    static Expected<Scene, Error> create(
        const glm::uvec2 &size,
        const glm::vec4 &background_color = glm::vec4(1.0f),
        std::optional<int> samples = 4,
        int depth_peeling_passes = 3
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
        const glm::uvec2 &size,
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

class Video
{
public:

    /**
     * @brief Creates a Video.
     *
     * @param file_name Output filename.
     * The file format will be figured out from the extension.
     *
     * @param size Width and height in pixels.
     *
     * @param frame_rate Frame rate in frames per second.
     *
     * @param bit_rate Bit rate in bits per second.
     *
     * @param intermediate_yuv420p_conversion
     * Whether to insert an additional conversion step for each frame creation.
     * Intermediate YUV420P conversion is necessary for
     * correct gif creation. Without it, gifs have strange colors.
     * The rendered texture is always converted to a pixel format
     * which is the best (and supported) for the current video format.
     * This option introduces an intermedia conversion between
     * the conversion from the rendered texture to video frame.
     * This option is not necessary for regular mp4, where the
     * destination pixel format will be YUV420P anyway.
     * In these cases, this argument has no effect.
     * This can be turned off for more performant video creation.
     *
     * @return A Video object if it is successful, an Error otherwise.
     */
    static Expected<Video, Error> create(
        const std::string &file_name,
        const glm::uvec2 &size,
        const unsigned int frame_rate = 30,
        const int64_t bit_rate = 5000000,
        const bool intermediate_yuv420p_conversion = true
    );

    Video(Video &&other);
    Video &operator=(Video &&other);

    void render(
        std::shared_ptr<const RenderedScene> rendered_scene,
        const RenderMode = RenderMode::fill
    );

    ~Video();

    Video(const Video &other) = delete;
    Video &operator=(const Video &other) = delete;

    Video() = delete;

private:

    class Impl;
    std::unique_ptr<Impl> impl;

    Video(std::unique_ptr<Impl> &&impl);
};
}

#endif
