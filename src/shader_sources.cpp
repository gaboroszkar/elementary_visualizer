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

const GlShaderSource &quad_multisampled_fragment_shader_source()
{
    static GlShaderSource source(
        GL_FRAGMENT_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform ivec2 scene_size;
uniform sampler2DMS texture_slot;

layout (location = 0) in vec2 texture_coordinate_in;

layout (location = 0) out vec4 color_out;

void main()
{
    int x = int(texture_coordinate_in.x * scene_size.x);
    int y = int(texture_coordinate_in.y * scene_size.y);
    color_out = texelFetch(texture_slot, ivec2(x, y), gl_SampleID);
}

)")
    );
    return source;
}

const GlShaderSource &depth_peeling_fragment_shader_source()
{
    static GlShaderSource source(
        GL_FRAGMENT_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform bool depth_peeling_first_pass;
uniform bool depth_peeling_multisampled;
uniform sampler2D depth_peeling_texture_slot;
uniform sampler2DMS depth_peeling_texture_slot_multisampled;
uniform ivec2 scene_size;

void discard_if_close_fragment(float peeled_depth)
{
    if (gl_FragCoord.z <= peeled_depth)
        discard;
}

void depth_peeling_discard()
{
    if (!depth_peeling_first_pass)
    {
        if (depth_peeling_multisampled)
        {
            // We resolve Z fighting for the depth peeling pass by
            // checking whether there is any depth peeling sample
            // which is further from to the scene than our current
            // fragment's depth. If yes, we discard that fragment.
            for (int sample_id = 0; sample_id < gl_NumSamples; ++sample_id)
                discard_if_close_fragment(
                    texelFetch(depth_peeling_texture_slot_multisampled, ivec2(gl_FragCoord.xy), sample_id).r
                );
        }
        else
        {
            discard_if_close_fragment(
                texture(depth_peeling_texture_slot, vec2(gl_FragCoord.x / scene_size.x, gl_FragCoord.y / scene_size.y)).r
            );
        }
    }
}

)")
    );
    return source;
}

const GlShaderSource &linesegments_vertex_shader_source()
{
    static GlShaderSource source(
        GL_VERTEX_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 start_position_in;
layout (location = 1) in vec4 start_color_in;
layout (location = 2) in vec3 end_position_in;
layout (location = 3) in vec4 end_color_in;
layout (location = 4) in float width_in;

layout (location = 0) out vec4 start_position_out;
layout (location = 1) out vec4 start_color_out;
layout (location = 2) out vec4 end_position_out;
layout (location = 3) out vec4 end_color_out;
layout (location = 4) out float width_out;

void main()
{
    start_position_out = projection * view * model * vec4(start_position_in, 1.0f);
    start_color_out = start_color_in;
    end_position_out = projection * view * model * vec4(end_position_in, 1.0f);
    end_color_out = end_color_in;
    width_out = width_in;
}

)")
    );
    return source;
}

const GlShaderSource &linesegments_geometry_shader_source()
{
    static GlShaderSource source(
        GL_GEOMETRY_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform ivec2 scene_size;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (location = 0) in vec4 start_position_in[];
layout (location = 1) in vec4 start_color_in[];
layout (location = 2) in vec4 end_position_in[];
layout (location = 3) in vec4 end_color_in[];
layout (location = 4) in float width_in[];

layout (location = 0) out vec4 color_out;

vec4 to_scene(vec4 v)
{
    // If the scene is a square,
    // the x, y coordinates go from -1 to +1.
    // If the scene is a rectangle,
    // the scene x goes from -aspect to aspect.
    v.x *= 0.5f * scene_size.x;
    v.y *= 0.5f * scene_size.y;
    v = vec4(v.xyz / v.w, v.w);
    return v;
}

vec4 from_scene(vec4 v)
{
    v = vec4(v.xyz * v.w, v.w);
    v.x /= 0.5f * scene_size.x;
    v.y /= 0.5f * scene_size.y;
    return v;
}

void emit_vertex(vec4 v, vec4 color)
{
    gl_Position = from_scene(v);
    color_out = color;
    EmitVertex();
}

vec4 rot_perpendicular(vec4 v)
{
    return vec4(-v.y, v.x, 0, 0);
}

float length2(vec4 v)
{
    return length(v.xy);
}

vec4 normalize2(vec4 v)
{
    return v / length2(v);
}

void main()
{
    vec4 p0 = to_scene(start_position_in[0]);
    vec4 p1 = to_scene(end_position_in[0]);

    float half_line_width = width_in[0] / 2.0f;

    vec4 normal_outer = half_line_width * normalize2(rot_perpendicular(p1 - p0));

    emit_vertex(p0 - normal_outer, start_color_in[0]);
    emit_vertex(p0 + normal_outer, start_color_in[0]);
    emit_vertex(p1 - normal_outer, end_color_in[0]);
    emit_vertex(p1 + normal_outer, end_color_in[0]);
    EndPrimitive();
}

)")
    );
    return source;
}

const GlShaderSource &linesegments_fragment_shader_source()
{
    static GlShaderSource source(
        GL_FRAGMENT_SHADER,
        std::string(SHADER_HEADER
                    R"(

layout (location = 0) in vec4 color_in;

layout (location = 0) out vec4 color_out;

void depth_peeling_discard();

void main()
{
    depth_peeling_discard();
    color_out = color_in;
}

)")
    );
    return source;
}
}
