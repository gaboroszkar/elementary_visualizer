#include <gl_shader_program.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace elementary_visualizer
{
bool shader_info(
    const GLuint index, const GLint params, const std::string &action
)
{
    if (params != GL_TRUE)
    {
        GLint log_max_length = 0;
        glGetShaderiv(index, GL_INFO_LOG_LENGTH, &log_max_length);

        std::vector<GLchar> info_log(log_max_length);
        glGetShaderInfoLog(
            index, log_max_length, &log_max_length, &info_log[0]
        );
        std::cout << "Error! Shader program " << action
                  << " error with index=" << index << "!\n";
        std::cout << info_log.data() << '\n';
    }

    return (params == GL_TRUE);
}

bool linker_error_handling(const GLuint index)
{
    GLint params;
    glGetProgramiv(index, GL_LINK_STATUS, &params);
    return shader_info(index, params, "link");
}

bool compilation_error_handling(const GLuint index)
{
    GLint params;
    glGetShaderiv(index, GL_COMPILE_STATUS, &params);
    return shader_info(index, params, "compile");
}

std::string buffer_to_string(const std::vector<GLchar> &buffer, GLsizei length)
{
    std::string s(buffer.cbegin(), buffer.cend());
    return s.substr(0, length);
}

class GlShader
{
public:

    static Expected<std::shared_ptr<GlShader>, Error> create(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const GlShaderSource &source
    )
    {
        if (!glfw_window)
            return Unexpected<Error>(Error());
        glfw_window->make_current_context();

        GLuint index = glCreateShader(source.type);

        const char *source_c_str = source.source.c_str();
        glShaderSource(index, 1, &source_c_str, nullptr);

        glCompileShader(index);
        if (!compilation_error_handling(index))
            return Unexpected<Error>(Error());

        return std::shared_ptr<GlShader>(new GlShader(glfw_window, index));
    }

    void attach(const GLuint program_index) const
    {
        glAttachShader(program_index, this->index);
    }

    ~GlShader()
    {
        this->glfw_window->make_current_context();
        glDeleteShader(index);
    }

    GlShader(GlShader &&other) = delete;
    GlShader &operator=(GlShader &&other) = delete;
    GlShader(const GlShader &other) = delete;
    GlShader &operator=(const GlShader &other) = delete;

private:

    GlShader(std::shared_ptr<WrappedGlfwWindow> glfw_window, const GLuint index)
        : glfw_window(glfw_window), index(index){};

    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    const GLuint index;
};

Expected<std::shared_ptr<GlShaderProgram>, Error> GlShaderProgram::create(
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    const std::vector<GlShaderSource> &sources
)
{
    std::vector<std::shared_ptr<GlShader>> compiled_shaders;
    for (const auto &source : sources)
    {
        Expected<std::shared_ptr<GlShader>, Error> compiled_shader =
            GlShader::create(glfw_window, source);
        if (!compiled_shader)
            return Unexpected<Error>(Error());
        compiled_shaders.push_back(compiled_shader.value());
    }

    if (!glfw_window)
        return Unexpected<Error>(Error());
    glfw_window->make_current_context();

    GLuint index = glCreateProgram();
    for (const auto &compiled_shader : compiled_shaders)
        compiled_shader->attach(index);

    glLinkProgram(index);
    if (!linker_error_handling(index))
        return Unexpected<Error>(Error());

    std::map<std::string, GLint> uniform_locations =
        GlShaderProgram::generate_uniform_locations(index);

    return std::shared_ptr<GlShaderProgram>(
        new GlShaderProgram(glfw_window, index, uniform_locations)
    );
}

void GlShaderProgram::use(bool make_context) const
{
    if (make_context)
        this->glfw_window->make_current_context();
    glUseProgram(this->index);
}

void GlShaderProgram::set_uniform(const std::string &name, const int value)
{
    if (this->uniform_locations.contains(name))
        glUniform1i(this->uniform_locations.at(name), value);
}

void GlShaderProgram::set_uniform(const std::string &name, const bool value)
{
    if (this->uniform_locations.contains(name))
        glUniform1i(this->uniform_locations.at(name), value ? 1 : 0);
}

void GlShaderProgram::set_uniform(const std::string &name, const float value)
{
    if (this->uniform_locations.contains(name))
        glUniform1f(this->uniform_locations.at(name), value);
}

void GlShaderProgram::set_uniform(
    const std::string &name, const glm::uvec2 &value
)
{
    if (this->uniform_locations.contains(name))
        glUniform2ui(this->uniform_locations.at(name), value.x, value.y);
}

void GlShaderProgram::set_uniform(
    const std::string &name, const glm::mat4 &value
)
{
    if (this->uniform_locations.contains(name))
        glUniformMatrix4fv(
            this->uniform_locations.at(name), 1, GL_FALSE, glm::value_ptr(value)
        );
}

GlShaderProgram::~GlShaderProgram()
{
    this->glfw_window->make_current_context();
    glDeleteProgram(this->index);
}

GlShaderProgram::GlShaderProgram(
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    const GLuint index,
    const std::map<std::string, GLint> &uniform_locations
)
    : glfw_window(glfw_window),
      index(index),
      uniform_locations(uniform_locations)
{}

std::map<std::string, GLint>
    GlShaderProgram::generate_uniform_locations(GLuint index)
{
    std::map<std::string, GLint> uniform_locations;

    GLint number_of_uniforms = 0;
    glGetProgramiv(index, GL_ACTIVE_UNIFORMS, &number_of_uniforms);

    GLint maximum_character_length = 0;
    glGetProgramiv(
        index, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maximum_character_length
    );

    if (number_of_uniforms > 0 && maximum_character_length > 0)
    {
        std::vector<GLchar> name_buffer(maximum_character_length);
        for (GLint uniform_index = 0; uniform_index != number_of_uniforms;
             ++uniform_index)
        {
            GLenum data_type;
            GLsizei character_length = 0;
            GLint size = 0;
            glGetActiveUniform(
                index,
                uniform_index,
                maximum_character_length,
                &character_length,
                &size,
                &data_type,
                &name_buffer[0]
            );
            GLint location = glGetUniformLocation(index, &name_buffer[0]);
            std::string uniform_name =
                buffer_to_string(name_buffer, character_length);
            uniform_locations[uniform_name] = location;
        }
    }

    return uniform_locations;
}
}
