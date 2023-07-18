#include <shader_sources.hpp>

namespace elementary_visualizer
{
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
}
