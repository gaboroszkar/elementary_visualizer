#ifndef ELEMENTARY_VISUALIZER_VISUALS_HPP
#define ELEMENTARY_VISUALIZER_VISUALS_HPP

#include <elementary_visualizer/elementary_visualizer.hpp>
#include <entity.hpp>
#include <gl_resources.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace elementary_visualizer
{
class LinesegmentsVisual::Impl
{
public:

    Impl(
        std::shared_ptr<Entity> entity,
        std::shared_ptr<GlLinesegments> linesegments
    );

    Impl(Impl &&other);
    Impl &operator=(Impl &&other);

    void render(const glm::ivec2 &scene_size) const;

    void set_linesegments_data(const std::vector<Linesegment> &linesegments_data
    );

    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

    ~Impl();

private:

    std::shared_ptr<Entity> entity;
    std::shared_ptr<GlLinesegments> linesegments;

public:

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};
}

#endif
