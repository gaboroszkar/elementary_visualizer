#include <shader_sources.hpp>

namespace elementary_visualizer
{
int line_cap_to_int(const LineCap cap)
{
    switch (cap)
    {
    case LineCap::butt:
        return 0;
    case LineCap::round:
        return 1;
    }

    return -1;
}

const GlShaderSource &line_cap_geometry_shader_source()
{
    static GlShaderSource source(
        GL_GEOMETRY_SHADER,
        std::string(SHADER_HEADER
                    R"(

void emit_vertex(vec4 v, vec4 color);

vec4 angled_v(float phi)
{
    return vec4(cos(phi), sin(phi), 0.0f, 0.0f);
}

void emit_line_cap(int cap, vec4 p0, vec4 p1, float width, vec4 color)
{
    if (cap == 1)
    {
        float pi = 3.14159265f;

        vec2 d = (p1 - p0).xy;
        vec2 n = normalize(vec2(-d.y, d.x));

        float half_line_width = width / 2.0f;

        mat4 r = half_line_width * mat4(
             n.x,  n.y, 0.0f, 0.0f,
            -n.y,  n.x, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        );

        // Important: if you change this, it's necessary to change
        // the max_vertices in the output layout where this is used!
        // max_vertices is additionally
        // +(3 * number_of_sides) whenever you call this function.
        int number_of_sides = 10;

        for (int i = 0; i != number_of_sides; ++i)
        {
            emit_vertex(p0, color);

            const float f = pi / number_of_sides;
            emit_vertex(p0 + r * angled_v(i * f), color);
            emit_vertex(p0 + r * angled_v((i + 1) * f), color);

            EndPrimitive();
        }
    }
}

)")
    );
    return source;
}

}
