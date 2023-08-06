#include <shader_sources.hpp>

namespace elementary_visualizer
{
const GlShaderSource &surface_vertex_shader_source()
{
    static GlShaderSource source(
        GL_VERTEX_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(binding = 1, std430) readonly buffer position_layout
{
    float position_in[];
};

layout(binding = 2, std430) readonly buffer color_normal_layout
{
    float color_normal_in[];
};

layout (location = 0) in uint position_index_in;
layout (location = 1) in uint color_normal_index_in;

layout (location = 0) out vec3 position_out;
layout (location = 1) out vec4 color_out;
layout (location = 2) out vec3 normal_out;

mat3 extract_rotation_and_scale(mat4 m)
{
    return mat3(m[0].xyz, m[1].xyz, m[2].xyz);
}

// Normals are transformed differently than
// the regular position coordinates.
// First, they do not translate.
// Second, to ensure that with scaling
// the normals are always perpendicular
// to a surface tangent, we need to invert
// and then transpose the model matrix.
// See <https://stackoverflow.com/q/13654401>.
mat3 get_normal_model(mat4 model)
{
    return transpose(inverse(extract_rotation_and_scale(model)));
}

void main()
{
    vec3 position = vec3(
        position_in[3 * position_index_in + 0],
        position_in[3 * position_index_in + 1],
        position_in[3 * position_index_in + 2]
    );
    vec4 color = vec4(
        color_normal_in[7 * color_normal_index_in + 0],
        color_normal_in[7 * color_normal_index_in + 1],
        color_normal_in[7 * color_normal_index_in + 2],
        color_normal_in[7 * color_normal_index_in + 3]
    );
    vec3 normal = vec3(
        color_normal_in[7 * color_normal_index_in + 4],
        color_normal_in[7 * color_normal_index_in + 5],
        color_normal_in[7 * color_normal_index_in + 6]
    );

    gl_Position = projection * view * model * vec4(position, 1.0f);
    position_out = vec3(model * vec4(position, 1.0f));
    color_out = color;
    normal_out = normalize(get_normal_model(model) * normal);
}

)")
    );
    return source;
}

const GlShaderSource &surface_fragment_shader_source()
{
    static GlShaderSource source(
        GL_FRAGMENT_SHADER,
        std::string(SHADER_HEADER
                    R"(

uniform vec3 eye;
uniform vec3 light_position;
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;

layout (location = 0) in vec3 position_in;
layout (location = 1) in vec4 color_in;
layout (location = 2) in vec3 normal_in;

layout (location = 0) out vec4 color_out;

void depth_peeling_discard();

void main()
{
    depth_peeling_discard();

    // The normal is not guaranteed to
    // be unit length after
    // interpolation between vertices.
    vec3 normal = normalize(normal_in);

    vec3 eye_direction = normalize(eye - position_in);

    // We need to find out which side of
    // the surface we see, and then decide
    // what is the normal based on that.
    if (dot(normal, -eye_direction) < 0.0f)
        normal = -normal;

    vec3 light_direction = normalize(light_position - position_in);

    float diffuse_magnitude = max(dot(normal, -light_direction), 0.0f);
    vec3 diffuse = diffuse_magnitude * diffuse_color;

    vec3 reflection_direction = reflect(-light_direction, normal);
    float specular_magnitude = pow(max(dot(eye_direction, reflection_direction), 0.0f), shininess);
    vec3 specular = specular_magnitude * specular_color;

    color_out = vec4((ambient_color + diffuse + specular) * color_in.rgb, color_in.a);
}

)")
    );
    return source;
}
}
