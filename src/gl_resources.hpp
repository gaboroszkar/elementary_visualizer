#ifndef ELEMENTARY_VISUALIZER_GL_RESOURCES_HPP
#define ELEMENTARY_VISUALIZER_GL_RESOURCES_HPP

#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glad/gl.h>
#include <glfw_resources.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <optional>

namespace elementary_visualizer
{
class GlTexture
{
public:

    static Expected<std::shared_ptr<GlTexture>, Error> create(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const glm::uvec2 &size,
        bool depth,
        const std::optional<int> samples
    );

    void bind(bool make_context = true) const;
    void framebuffer_texture(bool make_context = true) const;

    glm::uvec2 get_size() const;
    void set_size(const glm::uvec2 &size);

    ~GlTexture();

    GlTexture(GlTexture &&other) = delete;
    GlTexture &operator=(GlTexture &&other) = delete;
    GlTexture(const GlTexture &other) = delete;
    GlTexture &operator=(const GlTexture &other) = delete;

private:

    GlTexture(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const GLuint index,
        const glm::uvec2 &size,
        const bool depth,
        const std::optional<int> samples
    );

    static GLenum target(const std::optional<int> &samples);
    GLenum target() const;
    static GLint internalformat(bool depth);
    GLint internalformat() const;
    static GLenum format(bool depth);
    GLenum format() const;

    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    const GLuint index;
    glm::uvec2 size;
    const bool depth;

public:

    const std::optional<int> samples;
};

enum class FrameBufferBindType
{
    read,
    draw,
    read_draw
};

class GlFramebuffer
{
public:

    static Expected<std::shared_ptr<GlFramebuffer>, Error>
        create(std::shared_ptr<WrappedGlfwWindow> glfw_window);

    void bind(
        bool make_context = true,
        const FrameBufferBindType framebuffer_bind_type =
            FrameBufferBindType::read_draw
    ) const;

    ~GlFramebuffer();

    GlFramebuffer(GlFramebuffer &&other) = delete;
    GlFramebuffer &operator=(GlFramebuffer &&other) = delete;
    GlFramebuffer(const GlFramebuffer &other) = delete;
    GlFramebuffer &operator=(const GlFramebuffer &other) = delete;

private:

    GlFramebuffer(
        std::shared_ptr<WrappedGlfwWindow> glfw_window, const GLuint index
    );

    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    const GLuint index;
};

class GlFramebufferTexture
{
public:

    static Expected<std::shared_ptr<GlFramebufferTexture>, Error> create(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const glm::uvec2 &size,
        const std::optional<int> samples
    );

    const std::shared_ptr<GlFramebuffer> framebuffer;
    const std::shared_ptr<GlTexture> texture;

    ~GlFramebufferTexture();

    GlFramebufferTexture(GlFramebufferTexture &&other) = delete;
    GlFramebufferTexture &operator=(GlFramebufferTexture &&other) = delete;
    GlFramebufferTexture(const GlFramebufferTexture &other) = delete;
    GlFramebufferTexture &operator=(const GlFramebufferTexture &other) = delete;

private:

    GlFramebufferTexture(
        std::shared_ptr<GlFramebuffer> framebuffer,
        std::shared_ptr<GlTexture> texture
    );
};

class GlVertexBuffer
{
public:

    static Expected<std::shared_ptr<GlVertexBuffer>, Error>
        create(std::shared_ptr<WrappedGlfwWindow> glfw_window);

    void bind(bool make_context = true) const;

    ~GlVertexBuffer();

    GlVertexBuffer(GlVertexBuffer &&other) = delete;
    GlVertexBuffer &operator=(GlVertexBuffer &&other) = delete;
    GlVertexBuffer(const GlVertexBuffer &other) = delete;
    GlVertexBuffer &operator=(const GlVertexBuffer &other) = delete;

private:

    GlVertexBuffer(
        std::shared_ptr<WrappedGlfwWindow> glfw_window, const GLuint index
    );

    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    const GLuint index;
};

class GlVertexArray
{
public:

    static Expected<std::shared_ptr<GlVertexArray>, Error>
        create(std::shared_ptr<WrappedGlfwWindow> glfw_window);

    void bind(bool make_context = true) const;

    ~GlVertexArray();

    GlVertexArray(GlVertexArray &&other) = delete;
    GlVertexArray &operator=(GlVertexArray &&other) = delete;
    GlVertexArray(const GlVertexArray &other) = delete;
    GlVertexArray &operator=(const GlVertexArray &other) = delete;

private:

    GlVertexArray(
        std::shared_ptr<WrappedGlfwWindow> glfw_window, const GLuint index
    );

    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    const GLuint index;
};

class GlQuad
{
public:

    static Expected<std::shared_ptr<GlQuad>, Error>
        create(std::shared_ptr<WrappedGlfwWindow> glfw_window);

    void render(bool make_context = true) const;

    ~GlQuad();

    GlQuad(GlQuad &&other) = delete;
    GlQuad &operator=(GlQuad &&other) = delete;
    GlQuad(const GlQuad &other) = delete;
    GlQuad &operator=(const GlQuad &other) = delete;

private:

    GlQuad(
        std::shared_ptr<GlVertexArray> vertex_array,
        std::shared_ptr<GlVertexBuffer> vertex_buffer
    );

    const std::shared_ptr<GlVertexArray> vertex_array;
    const std::shared_ptr<GlVertexBuffer> vertex_buffer;
};

class GlCircle
{
public:

    static Expected<std::shared_ptr<GlCircle>, Error>
        create(std::shared_ptr<WrappedGlfwWindow> glfw_window);

    void render(bool make_context = true) const;

    ~GlCircle();

    GlCircle(GlCircle &&other) = delete;
    GlCircle &operator=(GlCircle &&other) = delete;
    GlCircle(const GlCircle &other) = delete;
    GlCircle &operator=(const GlCircle &other) = delete;

private:

    GlCircle(
        std::shared_ptr<GlVertexArray> vertex_array,
        std::shared_ptr<GlVertexBuffer> vertex_buffer
    );

    const std::shared_ptr<GlVertexArray> vertex_array;
    const std::shared_ptr<GlVertexBuffer> vertex_buffer;

    static unsigned int number_of_sides;
};

class GlLinesegments
{
public:

    static Expected<std::shared_ptr<GlLinesegments>, Error> create(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const std::vector<Linesegment> &linesegments_data
    );

    void render(bool make_context = true) const;

    void set_linesegments_data(const std::vector<Linesegment> &linesegments_data
    );

    ~GlLinesegments();

    GlLinesegments(GlLinesegments &&other) = delete;
    GlLinesegments &operator=(GlLinesegments &&other) = delete;
    GlLinesegments(const GlLinesegments &other) = delete;
    GlLinesegments &operator=(const GlLinesegments &other) = delete;

private:

    GlLinesegments(
        std::shared_ptr<GlVertexArray> vertex_array,
        std::shared_ptr<GlVertexBuffer> vertex_buffer,
        const int number_of_linesegments
    );

    static std::unique_ptr<std::vector<float>> generate_vertex_buffer_data(
        const std::vector<Linesegment> &linesegments_data
    );

    const std::shared_ptr<GlVertexArray> vertex_array;
    const std::shared_ptr<GlVertexBuffer> vertex_buffer;
    int number_of_linesegments;
};

class GlLines
{
public:

    static Expected<std::shared_ptr<GlLines>, Error> create(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const std::vector<Vertex> &lines_data
    );

    void render(bool make_context = true) const;

    void set_lines_data(const std::vector<Vertex> &lines_data);

    ~GlLines();

    GlLines(GlLines &&other) = delete;
    GlLines &operator=(GlLines &&other) = delete;
    GlLines(const GlLines &other) = delete;
    GlLines &operator=(const GlLines &other) = delete;

private:

    GlLines(
        std::shared_ptr<GlVertexArray> vertex_array,
        std::shared_ptr<GlVertexBuffer> vertex_buffer,
        const int number_of_lines
    );

    static void add_empty_vertex(std::vector<float> &vertices);
    static void add_vertex(std::vector<float> &vertices, const Vertex &vertex);
    static std::unique_ptr<std::vector<float>>
        generate_vertex_buffer_data(const std::vector<Vertex> &lines_data);

    const std::shared_ptr<GlVertexArray> vertex_array;
    const std::shared_ptr<GlVertexBuffer> vertex_buffer;
    int number_of_lines;
};

class GlSurface
{
public:

    static Expected<std::shared_ptr<GlSurface>, Error> create(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const SurfaceData &surface_data
    );

    void render(bool make_context = true) const;

    void set_surface_data(const SurfaceData &surface_data);

    ~GlSurface();

    GlSurface(GlSurface &&other) = delete;
    GlSurface &operator=(GlSurface &&other) = delete;
    GlSurface(const GlSurface &other) = delete;
    GlSurface &operator=(const GlSurface &other) = delete;

private:

    GlSurface(
        std::shared_ptr<GlVertexArray> vertex_array,
        std::shared_ptr<GlVertexBuffer> vertex_buffer,
        const int number_of_vertices
    );

    static void add_vertex(
        std::vector<float> &vertices,
        const glm::vec3 &position,
        const glm::vec4 &color,
        const glm::vec3 &normal
    );
    static void add_triangle_vertex(
        std::vector<float> &vertices,
        const SurfaceData &data,
        const glm::uvec2 &ref,
        const glm::uvec2 &uv,
        const bool upper_triangle
    );
    static unsigned int generate_vertex_buffer_data(
        std::vector<float> &vertices, const SurfaceData &surface_data
    );

    const std::shared_ptr<GlVertexArray> vertex_array;
    const std::shared_ptr<GlVertexBuffer> vertex_buffer;
    unsigned int number_of_triangles;
};
}

#endif
