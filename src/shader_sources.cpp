#include <shader_sources.hpp>

namespace elementary_visualizer
{
const GlShaderSource &quad_vertex_shader_source()
{
    static GlShaderSource source(
        GL_VERTEX_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position_in;
layout (location = 1) in vec2 texture_coordinate_in;

layout (location = 0) out vec2 texture_coordinate_out;

void main()
{
    gl_Position = projection * view * model * vec4(position_in, 1.0f);
    texture_coordinate_out = texture_coordinate_in;
}

)")
    );
    return source;
}

const GlShaderSource &quad_fragment_shader_source()
{
    static GlShaderSource source(
        GL_FRAGMENT_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform sampler2D texture_slot;

layout (location = 0) in vec2 texture_coordinate_in;

layout (location = 0) out vec4 color_out;

void main()
{
    color_out = texture(texture_slot, texture_coordinate_in);
}

)")
    );
    return source;
}
}
