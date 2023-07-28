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

    void render(
        const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
    ) const;

    void set_linesegments_data(const std::vector<Linesegment> &linesegments_data
    );

    Impl(Impl &&other) = delete;
    Impl &operator=(Impl &&other) = delete;
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
    bool projection_aspect_correction;
};

class LinesVisual::Impl
{
public:

    Impl(
        std::shared_ptr<Entity> entity,
        std::shared_ptr<GlLines> lines,
        const float width
    );

    void render(
        const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
    ) const;

    void set_lines_data(const std::vector<Vertex> &lines_data);

    Impl(Impl &&other) = delete;
    Impl &operator=(Impl &&other) = delete;
    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

    ~Impl();

private:

    std::shared_ptr<Entity> entity;
    std::shared_ptr<GlLines> lines;

public:

    float width;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    bool projection_aspect_correction;
};

class SurfaceVisual::Impl
{
public:

    Impl(std::shared_ptr<Entity> entity, std::shared_ptr<GlSurface> surface);

    void render(
        const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
    ) const;

    void set_surface_data(const SurfaceData &surface_data);

    Impl(Impl &&other) = delete;
    Impl &operator=(Impl &&other) = delete;
    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

    ~Impl();

private:

    std::shared_ptr<Entity> entity;
    std::shared_ptr<GlSurface> surface;

public:

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    bool projection_aspect_correction;

    std::optional<glm::vec3> light_position;
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    float shininess;
};

class CircleVisual::Impl
{
public:

    Impl(std::shared_ptr<Entity> entity, const glm::vec4 &color);

    void render(
        const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
    ) const;

    Impl(Impl &&other) = delete;
    Impl &operator=(Impl &&other) = delete;
    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

    ~Impl();

private:

    std::shared_ptr<Entity> entity;

public:

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    bool projection_aspect_correction;
    glm::vec4 color;
};
}

#endif
