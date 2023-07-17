#include <gl_resources.hpp>

namespace elementary_visualizer
{
Expected<std::shared_ptr<GlTexture>, Error> GlTexture::create(
    std::shared_ptr<WrappedGlfwWindow> glfw_window, const glm::ivec2 &size
)
{
    if (!glfw_window)
        return Unexpected<Error>(Error());
    glfw_window->make_current_context();

    GLuint index;
    glGenTextures(1, &index);

    const GLenum target = GlTexture::target();

    glBindTexture(target, index);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const GLint internalformat = GlTexture::internalformat();
    const GLenum format = GlTexture::format();

    glTexImage2D(
        target, 0, internalformat, size.x, size.y, 0, format, GL_FLOAT, nullptr
    );

    return std::shared_ptr<GlTexture>(new GlTexture(glfw_window, index, size));
}

void GlTexture::bind(bool make_context) const
{
    if (make_context)
        this->glfw_window->make_current_context();
    glBindTexture(this->target(), this->index);
}

void GlTexture::framebuffer_texture(bool make_context) const
{
    if (make_context)
        this->glfw_window->make_current_context();
    const GLenum attachment = GL_COLOR_ATTACHMENT0;
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, attachment, this->target(), this->index, 0
    );
}

glm::ivec2 GlTexture::get_size() const
{
    return this->size;
}

void GlTexture::set_size(const glm::ivec2 &size)
{
    this->bind();
    glTexImage2D(
        this->target(),
        0,
        this->internalformat(),
        size.x,
        size.y,
        0,
        this->format(),
        GL_FLOAT,
        nullptr
    );
    this->size = size;
}

GlTexture::~GlTexture()
{
    this->glfw_window->make_current_context();
    glDeleteTextures(1, &this->index);
}

GlTexture::GlTexture(
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    const GLuint index,
    const glm::ivec2 &size
)
    : glfw_window(glfw_window), index(index), size(size)
{}

GLenum GlTexture::target()
{
    return GL_TEXTURE_2D;
}

GLint GlTexture::internalformat()
{
    return GL_RGBA32F;
}

GLenum GlTexture::format()
{
    return GL_RGBA;
}

Expected<std::shared_ptr<GlFramebuffer>, Error>
    GlFramebuffer::create(std::shared_ptr<WrappedGlfwWindow> glfw_window)
{
    if (!glfw_window)
        return Unexpected<Error>(Error());
    glfw_window->make_current_context();

    GLuint index;
    glGenFramebuffers(1, &index);
    glBindFramebuffer(GL_FRAMEBUFFER, index);

    return std::shared_ptr<GlFramebuffer>(new GlFramebuffer(glfw_window, index)
    );
}

void GlFramebuffer::bind(bool make_context) const
{
    if (make_context)
        this->glfw_window->make_current_context();
    glBindFramebuffer(GL_FRAMEBUFFER, this->index);
}

GlFramebuffer::~GlFramebuffer()
{
    this->glfw_window->make_current_context();
    glDeleteTextures(1, &this->index);
}

GlFramebuffer::GlFramebuffer(
    std::shared_ptr<WrappedGlfwWindow> glfw_window, const GLuint index
)
    : glfw_window(glfw_window), index(index)
{}

Expected<std::shared_ptr<GlFramebufferTexture>, Error>
    GlFramebufferTexture::create(
        std::shared_ptr<WrappedGlfwWindow> glfw_window, const glm::ivec2 &size
    )
{
    if (!glfw_window)
        return Unexpected<Error>(Error());
    glfw_window->make_current_context();

    Expected<std::shared_ptr<GlFramebuffer>, Error> framebuffer =
        GlFramebuffer::create(glfw_window);
    if (!framebuffer)
        return Unexpected<Error>(Error());
    framebuffer.value()->bind();

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    Expected<std::shared_ptr<GlTexture>, Error> texture =
        GlTexture::create(glfw_window, size);
    if (!texture)
        return Unexpected<Error>(Error());
    texture.value()->bind();
    texture.value()->framebuffer_texture(false);

    GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
    {
        return Unexpected<Error>(Error());
    }

    return std::shared_ptr<GlFramebufferTexture>(
        new GlFramebufferTexture(framebuffer.value(), texture.value())
    );
}

GlFramebufferTexture::~GlFramebufferTexture() {}

GlFramebufferTexture::GlFramebufferTexture(
    std::shared_ptr<GlFramebuffer> framebuffer,
    std::shared_ptr<GlTexture> texture
)
    : framebuffer(framebuffer), texture(texture)
{}

}
