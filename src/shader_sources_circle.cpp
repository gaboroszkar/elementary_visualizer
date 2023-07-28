#include <shader_sources.hpp>

namespace elementary_visualizer
{
const GlShaderSource &circle_vertex_shader_source()
{
    static GlShaderSource source(
        GL_VERTEX_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position_in;

void main()
{
    gl_Position = projection * view * model * vec4(position_in, 1.0f);
}

)")
    );
    return source;
}

const GlShaderSource &circle_fragment_shader_source()
{
    static GlShaderSource source(
        GL_FRAGMENT_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform vec4 color;

layout (location = 0) out vec4 color_out;

void depth_peeling_discard();

void main()
{
    depth_peeling_discard();

    color_out = color;
}

)")
    );
    return source;
}
}
