#include <shader_sources.hpp>

namespace elementary_visualizer
{
const GlShaderSource &lines_vertex_shader_source()
{
    static GlShaderSource source(
        GL_VERTEX_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position_in;
layout (location = 1) in vec4 color_in;

layout (location = 0) out vec4 position_out;
layout (location = 1) out vec4 color_out;

void main()
{
    position_out = projection * view * model * vec4(position_in, 1.0f);
    color_out = color_in;
}

)")
    );
    return source;
}

const GlShaderSource &lines_geometry_shader_source()
{
    static GlShaderSource source(
        GL_GEOMETRY_SHADER,
        std::string(SHADER_HEADER
                    R"(
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 12) out;

uniform ivec2 scene_size;
uniform float line_width;

layout (location = 0) in vec4 position_in[];
layout (location = 1) in vec4 color_in[];

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

float cross2(vec4 v0, vec4 v1)
{
    return v0.x * v1.y - v0.y * v1.x;
}

float square(float n)
{
    return n * n;
}

float dot2(vec4 v1, vec4 v2)
{
    return dot(v1.xy, v2.xy);
}

struct tip
{
    vec4 left;
    vec4 right;
    vec4 miter;
    vec4 outer;
};

tip calculate_tip(vec4 p0, vec4 p1, vec4 p2, float line_width)
{
    float half_line_width = 0.5f * line_width;

    vec4 line0 = p1 - p0;
    vec4 line1 = p2 - p1;

    vec4 direction0 = normalize2(line0);
    vec4 direction1 = normalize2(line1);

    int direction_factor = (cross2(direction0, direction1) > 0) ? -1 : 1;

    vec4 normal_outer1 = direction_factor * rot_perpendicular(direction1);

    vec4 miter_outer_direction = direction_factor * normalize2(rot_perpendicular(direction0 + direction1));
    vec4 tip_miter = half_line_width * miter_outer_direction;

    float leg_squared = square(1 / dot2(normal_outer1, miter_outer_direction)) - 1.0f;
    if (leg_squared < 0.0f)
        leg_squared = 0.0f;
    float inner_sides_intersection_to_direction1_length = half_line_width * sqrt(leg_squared);
    float tip_inner_absolute_to_direction1_length =
        min(inner_sides_intersection_to_direction1_length, 0.5f * length(line1.xy));
    vec4 tip_inner = - half_line_width * normal_outer1 + tip_inner_absolute_to_direction1_length * direction1;

    vec4 tip_outer = half_line_width * normal_outer1;

    vec4 tip_left;
    vec4 tip_right;
    if (direction_factor == -1)
    {
        tip_left = tip_inner;
        tip_right = tip_outer;
    }
    else
    {
        tip_left = tip_outer;
        tip_right = tip_inner;
    }

    return tip(tip_left, tip_right, tip_miter, tip_outer);
}

void main()
{
    vec4 p0 = to_scene(position_in[0]);
    vec4 p1 = to_scene(position_in[1]);
    vec4 p2 = to_scene(position_in[2]);
    vec4 p3 = to_scene(position_in[3]);

    if (!isnan(position_in[0].x))
    {
        tip tip0 = calculate_tip(p0, p1, p2, line_width);

        emit_vertex(p1 + tip0.miter, color_in[1]);
        emit_vertex(p1, color_in[1]);
        emit_vertex(p1 + tip0.outer, color_in[1]);
        EndPrimitive();

        emit_vertex(p1, color_in[1]);
        emit_vertex(p1 + tip0.left, color_in[1]);
        emit_vertex(p1 + tip0.right, color_in[1]);
    }
    else
    {
        vec4 normal_outer = 0.5f * line_width * normalize2(rot_perpendicular(p2 - p1));
        emit_vertex(p1 + normal_outer, color_in[1]);
        emit_vertex(p1 - normal_outer, color_in[1]);
    }

    if (!isnan(position_in[3].x))
    {
        tip tip1 = calculate_tip(p3, p2, p1, line_width);

        emit_vertex(p2 + tip1.right, color_in[2]);
        emit_vertex(p2 + tip1.left, color_in[2]);
        emit_vertex(p2, color_in[2]);

        EndPrimitive();

        emit_vertex(p2 + tip1.miter, color_in[2]);
        emit_vertex(p2, color_in[2]);
        emit_vertex(p2 + tip1.outer, color_in[2]);
    }
    else
    {
        vec4 normal_outer = 0.5f * line_width * normalize2(rot_perpendicular(p2 - p1));
        emit_vertex(p2 + normal_outer, color_in[2]);
        emit_vertex(p2 - normal_outer, color_in[2]);
    }

    EndPrimitive();
}

)")
    );
    return source;
}

const GlShaderSource &lines_fragment_shader_source()
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
