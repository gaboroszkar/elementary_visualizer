#ifndef ELEMENTARY_VISUALIZER_TEST_UTILITIES_HPP
#define ELEMENTARY_VISUALIZER_TEST_UTILITIES_HPP

#include <gl_resources.hpp>

std::size_t rendered_scene_hash(
    const std::shared_ptr<const elementary_visualizer::GlTexture>
        &rendered_scene,
    const glm::uvec2 &size,
    const bool debug = false
);

#endif
