#ifndef ELEMENTARY_VISUALIZER_GL_SHADER_PROGRAM_HPP
#define ELEMENTARY_VISUALIZER_GL_SHADER_PROGRAM_HPP

#include <elementary_visualizer/elementary_visualizer.hpp>
#include <glad/gl.h>
#include <glfw_resources.hpp>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define _STRINGIFY(S) #S
#define STRINGIFY(S) _STRINGIFY(S)
#define SHADER_HEADER                                                          \
    "#version " STRINGIFY(GL_VERSION_MAJOR) STRINGIFY(GL_VERSION_MINOR) "0 "   \
                                                                        "core"

namespace elementary_visualizer
{
struct GlShaderSource
{
    GLenum type;
    std::string source;
    GlShaderSource(GLenum type, const std::string &source)
        : type(type), source(source)
    {}
};

class GlShaderProgram
{
public:

    static Expected<std::shared_ptr<GlShaderProgram>, Error> create(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const std::vector<GlShaderSource> &sources
    );

    void use(bool make_context = true) const;

    void set_uniform(const std::string &name, const int value);
    void set_uniform(const std::string &name, const bool value);
    void set_uniform(const std::string &name, const float value);
    void set_uniform(const std::string &name, const glm::uvec2 &value);
    void set_uniform(const std::string &name, const glm::vec3 &value);
    void set_uniform(const std::string &name, const glm::mat4 &value);

    ~GlShaderProgram();

    GlShaderProgram(GlShaderProgram &&other) = delete;
    GlShaderProgram &operator=(GlShaderProgram &&other) = delete;
    GlShaderProgram(const GlShaderProgram &other) = delete;
    GlShaderProgram &operator=(const GlShaderProgram &other) = delete;

private:

    GlShaderProgram(
        std::shared_ptr<WrappedGlfwWindow> glfw_window,
        const GLuint index,
        const std::map<std::string, GLint> &uniform_locations
    );

    static std::map<std::string, GLint> generate_uniform_locations(GLuint index
    );

    std::shared_ptr<WrappedGlfwWindow> glfw_window;
    const GLuint index;
    const std::map<std::string, GLint> uniform_locations;
};

}

#endif
