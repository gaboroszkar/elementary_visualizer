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
        std::shared_ptr<WrappedGlfwWindow> glfw_window, const glm::ivec2 &size
    );

    void bind(bool make_context = true) const;
    void framebuffer_texture(bool make_context = true) const;

    glm::ivec2 get_size() const;
    void set_size(const glm::ivec2 &size);

    ~GlTexture();

    GlTexture(GlTexture &&other) = delete;
    GlTexture &operator=(GlTexture &&other) = delete;
    GlTexture(const GlTexture &other) = delete;
    GlTexture &operator=(const GlTexture &other) = delete;

private:

    GlTexture(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const GLuint index,
        const glm::ivec2 &size
    );

    static GLenum target();
    static GLint internalformat();
    static GLenum format();

    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    const GLuint index;
    glm::ivec2 size;
};

class GlFramebuffer
{
public:

    static Expected<std::shared_ptr<GlFramebuffer>, Error>
        create(std::shared_ptr<WrappedGlfwWindow> glfw_window);

    void bind(bool make_context = true) const;

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
        std::shared_ptr<WrappedGlfwWindow> glfw_window, const glm::ivec2 &size
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
}

#endif
