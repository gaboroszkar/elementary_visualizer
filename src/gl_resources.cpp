#include <gl_resources.hpp>

namespace elementary_visualizer
{
Expected<std::shared_ptr<GlTexture>, Error> GlTexture::create(
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    const glm::uvec2 &size,
    const bool depth,
    const std::optional<int> samples
)
{
    if (!glfw_window)
        return Unexpected<Error>(Error());
    glfw_window->make_current_context();

    GLuint index;
    glGenTextures(1, &index);

    const GLenum target = GlTexture::target(samples);

    glBindTexture(target, index);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const GLint internalformat = GlTexture::internalformat(depth);
    const GLenum format = GlTexture::format(depth);

    if (samples)
        glTexImage2DMultisample(
            target, samples.value(), internalformat, size.x, size.y, GL_TRUE
        );
    else
        glTexImage2D(
            target,
            0,
            internalformat,
            size.x,
            size.y,
            0,
            format,
            GL_FLOAT,
            nullptr
        );

    return std::shared_ptr<GlTexture>(
        new GlTexture(glfw_window, index, size, depth, samples)
    );
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
    const GLenum attachment =
        this->depth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0;
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, attachment, this->target(), this->index, 0
    );
}

glm::uvec2 GlTexture::get_size() const
{
    return this->size;
}

void GlTexture::set_size(const glm::uvec2 &size)
{
    this->bind();
    if (this->samples)
        glTexImage2DMultisample(
            this->target(),
            samples.value(),
            this->internalformat(),
            size.x,
            size.y,
            GL_TRUE
        );
    else
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
    const glm::uvec2 &size,
    bool depth,
    const std::optional<int> samples
)
    : glfw_window(glfw_window),
      index(index),
      size(size),
      depth(depth),
      samples(samples)
{}

GLenum GlTexture::target(const std::optional<int> &samples)
{
    return samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

GLenum GlTexture::target() const
{
    return GlTexture::target(this->samples);
}

GLint GlTexture::internalformat(bool depth)
{
    return depth ? GL_DEPTH_COMPONENT32F : GL_RGBA32F;
}

GLint GlTexture::internalformat() const
{
    return GlTexture::internalformat(this->depth);
}

GLenum GlTexture::format(bool depth)
{
    return depth ? GL_DEPTH_COMPONENT : GL_RGBA;
}

GLenum GlTexture::format() const
{
    return GlTexture::format(this->depth);
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

void GlFramebuffer::bind(
    bool make_context, const FrameBufferBindType framebuffer_bind_type
) const
{
    if (make_context)
        this->glfw_window->make_current_context();
    switch (framebuffer_bind_type)
    {
    case FrameBufferBindType::read:
        glBindFramebuffer(GL_READ_FRAMEBUFFER, this->index);
        break;
    case FrameBufferBindType::draw:
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->index);
        break;
    case FrameBufferBindType::read_draw:
        glBindFramebuffer(GL_FRAMEBUFFER, this->index);
        break;
    }
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
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const glm::uvec2 &size,
        const std::optional<int> samples
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
        GlTexture::create(glfw_window, size, false, samples);
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

Expected<std::shared_ptr<GlVertexBuffer>, Error>
    GlVertexBuffer::create(std::shared_ptr<WrappedGlfwWindow> glfw_window)
{
    if (!glfw_window)
        return Unexpected<Error>(Error());
    glfw_window->make_current_context();

    GLuint index;
    glGenBuffers(1, &index);
    return std::shared_ptr<GlVertexBuffer>(
        new GlVertexBuffer(glfw_window, index)
    );
}

void GlVertexBuffer::bind(bool make_context) const
{
    if (make_context)
        this->glfw_window->make_current_context();
    glBindBuffer(GL_ARRAY_BUFFER, this->index);
}

GlVertexBuffer::~GlVertexBuffer()
{
    this->glfw_window->make_current_context();
    glDeleteBuffers(1, &this->index);
}

GlVertexBuffer::GlVertexBuffer(
    std::shared_ptr<WrappedGlfwWindow> glfw_window, const GLuint index
)
    : glfw_window(glfw_window), index(index)
{}

Expected<std::shared_ptr<GlVertexArray>, Error>
    GlVertexArray::create(std::shared_ptr<WrappedGlfwWindow> glfw_window)
{
    if (glfw_window)
        glfw_window->make_current_context();

    GLuint index;
    glGenVertexArrays(1, &index);
    return std::shared_ptr<GlVertexArray>(new GlVertexArray(glfw_window, index)
    );
}

void GlVertexArray::bind(bool make_context) const
{
    if (make_context)
        this->glfw_window->make_current_context();
    glBindVertexArray(this->index);
}

GlVertexArray::~GlVertexArray()
{
    this->glfw_window->make_current_context();
    glDeleteVertexArrays(1, &this->index);
}

GlVertexArray::GlVertexArray(
    std::shared_ptr<WrappedGlfwWindow> glfw_window, const GLuint index
)
    : glfw_window(glfw_window), index(index)
{}

Expected<std::shared_ptr<GlQuad>, Error>
    GlQuad::create(std::shared_ptr<WrappedGlfwWindow> glfw_window)
{
    Expected<std::shared_ptr<GlVertexArray>, Error> vertex_array =
        GlVertexArray::create(glfw_window);
    if (!vertex_array)
        return Unexpected<Error>(Error());
    vertex_array.value()->bind();

    Expected<std::shared_ptr<GlVertexBuffer>, Error> vertex_buffer =
        GlVertexBuffer::create(glfw_window);
    if (!vertex_buffer)
        return Unexpected<Error>(Error());
    vertex_buffer.value()->bind();

    // Position 3 coordinates and texture uv 2 coordinates.
    std::vector<float> vertices = {
        // Lower left corner.
        -1.0f,
        -1.0f,
        0.0f,
        0.0f,
        0.0f,
        // Lower right corner.
        +1.0f,
        -1.0f,
        0.0f,
        1.0f,
        0.0f,
        // Upper left corner.
        -1.0f,
        +1.0f,
        0.0f,
        0.0f,
        1.0f,
        // Upper right corner.
        +1.0f,
        +1.0f,
        0.0f,
        1.0f,
        1.0f,
    };

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices.size(),
        &vertices[0],
        GL_STATIC_DRAW
    );

    // Configure the vertex attribute so that OpenGL knows how to read the
    // vertex buffer.
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, (3 + 2) * sizeof(float), nullptr
    );
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        (3 + 2) * sizeof(float),
        reinterpret_cast<void *>(3 * sizeof(float))
    );

    // Enable the vertex attribute.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return std::shared_ptr<GlQuad>(
        new GlQuad(vertex_array.value(), vertex_buffer.value())
    );
}

void GlQuad::render(bool make_context) const
{
    this->vertex_array->bind(make_context);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

GlQuad::~GlQuad() {}

GlQuad::GlQuad(
    std::shared_ptr<GlVertexArray> vertex_array,
    std::shared_ptr<GlVertexBuffer> vertex_buffer
)
    : vertex_array(vertex_array), vertex_buffer(vertex_buffer)
{}

Expected<std::shared_ptr<GlLinesegments>, Error> GlLinesegments::create(
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    const std::vector<Linesegment> &linesegments_data
)
{
    Expected<std::shared_ptr<GlVertexArray>, Error> vertex_array =
        GlVertexArray::create(glfw_window);
    if (!vertex_array)
        return Unexpected<Error>(Error());
    vertex_array.value()->bind();

    Expected<std::shared_ptr<GlVertexBuffer>, Error> vertex_buffer =
        GlVertexBuffer::create(glfw_window);
    if (!vertex_buffer)
        return Unexpected<Error>(Error());
    vertex_buffer.value()->bind();

    std::unique_ptr<std::vector<float>> vertices =
        GlLinesegments::generate_vertex_buffer_data(linesegments_data);
    if (!vertices)
        return Unexpected<Error>(Error());

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices->size(),
        &(*vertices)[0],
        GL_DYNAMIC_DRAW
    );
    const int number_of_linesegments = linesegments_data.size();

    // Configure the vertex attribute so that OpenGL knows how to read the
    // vertex buffer. 3 floats for start position; 4 floats for start color; 3
    // floats for end position; 4 floats for end color; 1 float for width.
    const int stride = 3 + 4 + 3 + 4 + 1;
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void *>(0 * sizeof(float))
    );
    glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void *>(3 * sizeof(float))
    );
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void *>(7 * sizeof(float))
    );
    glVertexAttribPointer(
        3,
        4,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void *>(10 * sizeof(float))
    );
    glVertexAttribPointer(
        4,
        1,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void *>(14 * sizeof(float))
    );

    // Enable the vertex attribute.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    return std::shared_ptr<GlLinesegments>(new GlLinesegments(
        vertex_array.value(), vertex_buffer.value(), number_of_linesegments
    ));
}

void GlLinesegments::render(bool make_context) const
{
    this->vertex_array->bind(make_context);
    glDrawArrays(GL_POINTS, 0, this->number_of_linesegments);
}

void GlLinesegments::set_linesegments_data(
    const std::vector<Linesegment> &linesegments_data
)
{
    std::unique_ptr<std::vector<float>> vertices =
        GlLinesegments::generate_vertex_buffer_data(linesegments_data);
    if (!vertices)
        return;

    this->vertex_array->bind();
    this->vertex_buffer->bind();

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices->size(),
        &(*vertices)[0],
        GL_DYNAMIC_DRAW
    );
    this->number_of_linesegments = linesegments_data.size();
}

GlLinesegments::~GlLinesegments() {}

GlLinesegments::GlLinesegments(
    std::shared_ptr<GlVertexArray> vertex_array,
    std::shared_ptr<GlVertexBuffer> vertex_buffer,
    const int number_of_linesegments
)
    : vertex_array(vertex_array),
      vertex_buffer(vertex_buffer),
      number_of_linesegments(number_of_linesegments)
{}

std::unique_ptr<std::vector<float>> GlLinesegments::generate_vertex_buffer_data(
    const std::vector<Linesegment> &linesegments_data
)
{
    // Each linesegment is represented by
    // 2 points (2 * 3 floats), and 2 colors (2 * 4 floats), and one width (1
    // float), overall 15 floats. This 15 floats is represented by one vertex in
    // GLSL input.
    std::unique_ptr<std::vector<float>> vertices =
        std::make_unique<std::vector<float>>();
    const int float_per_linesegment = 3 + 4 + 3 + 4 + 1;
    vertices->reserve(float_per_linesegment * linesegments_data.size());
    for (const auto linesegment : linesegments_data)
    {
        // Start position.
        vertices->push_back(linesegment.start.position.x); // 0
        vertices->push_back(linesegment.start.position.y); // 1
        vertices->push_back(linesegment.start.position.z); // 2
        // Start color.
        vertices->push_back(linesegment.start.color.r); // 3
        vertices->push_back(linesegment.start.color.g); // 4
        vertices->push_back(linesegment.start.color.b); // 5
        vertices->push_back(linesegment.start.color.a); // 6
        // End position.
        vertices->push_back(linesegment.end.position.x); // 7
        vertices->push_back(linesegment.end.position.y); // 8
        vertices->push_back(linesegment.end.position.z); // 9
        // End color.
        vertices->push_back(linesegment.end.color.r); // 10
        vertices->push_back(linesegment.end.color.g); // 11
        vertices->push_back(linesegment.end.color.b); // 12
        vertices->push_back(linesegment.end.color.a); // 13
        // Line width.
        vertices->push_back(linesegment.width); // 14
    }
    return vertices;
}

Expected<std::shared_ptr<GlLines>, Error> GlLines::create(
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    const std::vector<Vertex> &lines_data
)
{
    Expected<std::shared_ptr<GlVertexArray>, Error> vertex_array =
        GlVertexArray::create(glfw_window);
    if (!vertex_array)
        return Unexpected<Error>(Error());
    vertex_array.value()->bind();

    Expected<std::shared_ptr<GlVertexBuffer>, Error> vertex_buffer =
        GlVertexBuffer::create(glfw_window);
    if (!vertex_buffer)
        return Unexpected<Error>(Error());
    vertex_buffer.value()->bind();

    std::unique_ptr<std::vector<float>> vertices =
        GlLines::generate_vertex_buffer_data(lines_data);
    if (!vertices)
        return Unexpected<Error>(Error());

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices->size(),
        &(*vertices)[0],
        GL_DYNAMIC_DRAW
    );
    const int number_of_lines =
        (lines_data.size() < 2) ? 0 : (lines_data.size() - 1);

    // Configure the vertex attribute so that OpenGL knows how to read the
    // vertex buffer. 3 floats for position; 4 floats for color.
    const int stride = 3 + 4;
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void *>(0 * sizeof(float))
    );
    glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(float),
        reinterpret_cast<void *>(3 * sizeof(float))
    );

    // Enable the vertex attribute.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return std::shared_ptr<GlLines>(new GlLines(
        vertex_array.value(), vertex_buffer.value(), number_of_lines
    ));
}

void GlLines::render(bool make_context) const
{
    this->vertex_array->bind(make_context);
    glDrawArrays(GL_LINES_ADJACENCY, 0, 4 * this->number_of_lines);
}

void GlLines::set_lines_data(const std::vector<Vertex> &lines_data)
{
    std::unique_ptr<std::vector<float>> vertices =
        GlLines::generate_vertex_buffer_data(lines_data);
    if (!vertices)
        return;

    this->vertex_array->bind();
    this->vertex_buffer->bind();

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices->size(),
        &(*vertices)[0],
        GL_DYNAMIC_DRAW
    );
    this->number_of_lines = lines_data.size();
}

GlLines::~GlLines() {}

GlLines::GlLines(
    std::shared_ptr<GlVertexArray> vertex_array,
    std::shared_ptr<GlVertexBuffer> vertex_buffer,
    const int number_of_lines
)
    : vertex_array(vertex_array),
      vertex_buffer(vertex_buffer),
      number_of_lines(number_of_lines)
{}

// This function adds an "empty" vertex, signaling
// the beginning or the end of the line.
void GlLines::add_empty_vertex(std::vector<float> &vertices)
{
    // If 4 consequtive vertices start with this, or end with this
    // that represents the beginning or the end of the line.
    // It is a memory efficient way to store these properties.
    const float nan = std::numeric_limits<float>::quiet_NaN();
    for (int i = 0; i < 7; ++i)
        vertices.push_back(nan);
}

void GlLines::add_vertex(std::vector<float> &vertices, const Vertex &vertex)
{
    vertices.push_back(vertex.position.x);
    vertices.push_back(vertex.position.y);
    vertices.push_back(vertex.position.z);

    vertices.push_back(vertex.color.r);
    vertices.push_back(vertex.color.g);
    vertices.push_back(vertex.color.b);
    vertices.push_back(vertex.color.a);
}

std::unique_ptr<std::vector<float>>
    GlLines::generate_vertex_buffer_data(const std::vector<Vertex> &lines_data)
{
    // Each line is represented by
    // 4 vertices (4 * (3 + 4) floats), each of these vertices
    // contain 3 floats for position and 4 floats for color,
    // overall 28 floats.
    std::unique_ptr<std::vector<float>> vertices =
        std::make_unique<std::vector<float>>();
    if (lines_data.size() < 2)
        return vertices;

    const int float_per_line = 4 * (3 + 4);
    vertices->reserve(float_per_line * lines_data.size());

    GlLines::add_empty_vertex(*vertices);

    const auto end = lines_data.cend();
    for (auto it = lines_data.cbegin(); it != (end - 2); ++it)
    {
        // These are the last 3 points (out of 4) for the current segment.
        GlLines::add_vertex(*vertices, *(it + 0));
        GlLines::add_vertex(*vertices, *(it + 1));
        GlLines::add_vertex(*vertices, *(it + 2));

        // This is for the 0th point (out of 4) for the next segment.
        GlLines::add_vertex(*vertices, *(it + 0));
    }

    GlLines::add_vertex(*vertices, *(end - 2));
    GlLines::add_vertex(*vertices, *(end - 1));
    GlLines::add_empty_vertex(*vertices);

    return vertices;
}
}
