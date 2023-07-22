#ifndef ELEMENTARY_VISUALIZER_SHADER_SOURCES_HPP
#define ELEMENTARY_VISUALIZER_SHADER_SOURCES_HPP

#include <gl_shader_program.hpp>

namespace elementary_visualizer
{
const GlShaderSource &quad_vertex_shader_source();
const GlShaderSource &quad_fragment_shader_source();
const GlShaderSource &quad_multisampled_fragment_shader_source();

const GlShaderSource &depth_peeling_fragment_shader_source();

const GlShaderSource &linesegments_vertex_shader_source();
const GlShaderSource &linesegments_geometry_shader_source();
const GlShaderSource &linesegments_fragment_shader_source();

const GlShaderSource &lines_vertex_shader_source();
const GlShaderSource &lines_geometry_shader_source();
const GlShaderSource &lines_fragment_shader_source();

const GlShaderSource &surface_vertex_shader_source();
const GlShaderSource &surface_fragment_shader_source();
}

#endif
