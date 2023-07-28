#include <glm/gtc/matrix_inverse.hpp>
#include <scene.hpp>
#include <shader_sources.hpp>
#include <visuals.hpp>

namespace elementary_visualizer
{

glm::mat4 make_projection(
    const glm::mat4 &projection_in,
    const bool projection_aspect_correction,
    const glm::uvec2 &scene_size
)
{
    glm::mat4 projection = projection_in;
    if (projection_aspect_correction)
    {
        const float aspect =
            static_cast<float>(scene_size.x) / static_cast<float>(scene_size.y);
        projection[0][0] = projection[0][0] / aspect;
    }

    return projection;
}

LinesegmentsVisual::Impl::Impl(
    std::shared_ptr<Entity> entity,
    std::shared_ptr<GlLinesegments> linesegments,
    const LineCap cap
)
    : entity(entity),
      linesegments(linesegments),
      cap(cap),
      model(1.0f),
      view(1.0f),
      projection(1.0f),
      projection_aspect_correction(true)
{}

void LinesegmentsVisual::Impl::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    std::shared_ptr<GlShaderProgram> shader_program =
        this->entity->linesegments_shader_program;
    shader_program->use();

    depth_peeling_set_uniforms(shader_program, depth_peeling_data);

    shader_program->set_uniform("line_cap", line_cap_to_int(this->cap));

    shader_program->set_uniform("model", this->model);
    shader_program->set_uniform("view", this->view);
    shader_program->set_uniform(
        "projection",
        make_projection(
            this->projection, this->projection_aspect_correction, scene_size
        )
    );

    shader_program->set_uniform("scene_size", scene_size);

    this->linesegments->render(false);
}

void LinesegmentsVisual::Impl::set_linesegments_data(
    const std::vector<Linesegment> &linesegments_data
)
{
    this->linesegments->set_linesegments_data(linesegments_data);
}

LinesegmentsVisual::Impl::~Impl(){};

Expected<std::shared_ptr<LinesegmentsVisual>, Error> LinesegmentsVisual::create(
    const std::vector<Linesegment> &linesegments_data, const LineCap cap
)
{
    return Entity::ensure_initialized_and_get().and_then(
        [&linesegments_data, &cap](std::shared_ptr<Entity> entity
        ) -> Expected<std::shared_ptr<LinesegmentsVisual>, Error>
        {
            Expected<std::shared_ptr<GlLinesegments>, Error> linesegments =
                entity->create_linesegments(linesegments_data);
            if (!linesegments)
                return Unexpected<Error>(Error());

            std::unique_ptr<LinesegmentsVisual::Impl> impl(
                std::make_unique<LinesegmentsVisual::Impl>(
                    entity, linesegments.value(), cap
                )
            );
            return std::shared_ptr<LinesegmentsVisual>(
                new LinesegmentsVisual(std::move(impl))
            );
        }
    );
}

LinesegmentsVisual::LinesegmentsVisual(LinesegmentsVisual &&other)
    : impl(std::move(other.impl))
{}
LinesegmentsVisual &LinesegmentsVisual::operator=(LinesegmentsVisual &&other)
{
    this->impl = std::move(other.impl);
    return *this;
}

LinesegmentsVisual::LinesegmentsVisual(LinesegmentsVisual &other)
    : impl(std::move(other.impl))
{}
LinesegmentsVisual &LinesegmentsVisual::operator=(LinesegmentsVisual &other)
{
    this->impl = std::move(other.impl);
    return *this;
}

void LinesegmentsVisual::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    this->impl->render(scene_size, depth_peeling_data);
}

void LinesegmentsVisual::set_model(const glm::mat4 &model)
{
    this->impl->model = model;
}

void LinesegmentsVisual::set_view(const glm::mat4 &view)
{
    this->impl->view = view;
}

void LinesegmentsVisual::set_projection(const glm::mat4 &projection)
{
    this->impl->projection = projection;
}

void LinesegmentsVisual::set_projection_aspect_correction(
    const bool projection_aspect_correction
)
{
    this->impl->projection_aspect_correction = projection_aspect_correction;
}

void LinesegmentsVisual::set_linesegments_data(
    const std::vector<Linesegment> &linesegments_data
)
{
    this->impl->set_linesegments_data(linesegments_data);
}

void LinesegmentsVisual::set_cap(const LineCap cap)
{
    this->impl->cap = cap;
}

LinesegmentsVisual::~LinesegmentsVisual() {}

LinesegmentsVisual::LinesegmentsVisual(
    std::unique_ptr<LinesegmentsVisual::Impl> impl
)
    : impl(std::move(impl))
{}

LinesVisual::Impl::Impl(
    std::shared_ptr<Entity> entity,
    std::shared_ptr<GlLines> lines,
    const float width,
    const LineCap cap
)
    : entity(entity),
      lines(lines),
      width(width),
      cap(cap),
      model(1.0f),
      view(1.0f),
      projection(1.0f),
      projection_aspect_correction(true)
{}

void LinesVisual::Impl::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    std::shared_ptr<GlShaderProgram> shader_program =
        this->entity->lines_shader_program;
    shader_program->use();

    depth_peeling_set_uniforms(shader_program, depth_peeling_data);

    shader_program->set_uniform("line_width", this->width);
    shader_program->set_uniform("line_cap", line_cap_to_int(this->cap));

    shader_program->set_uniform("model", this->model);
    shader_program->set_uniform("view", this->view);
    shader_program->set_uniform(
        "projection",
        make_projection(
            this->projection, this->projection_aspect_correction, scene_size
        )
    );

    shader_program->set_uniform("scene_size", scene_size);

    this->lines->render(false);
}

void LinesVisual::Impl::set_lines_data(const std::vector<Vertex> &lines_data)
{
    this->lines->set_lines_data(lines_data);
}

LinesVisual::Impl::~Impl(){};

Expected<std::shared_ptr<LinesVisual>, Error> LinesVisual::create(
    const std::vector<Vertex> &lines_data, const float width, const LineCap cap
)
{
    return Entity::ensure_initialized_and_get().and_then(
        [&lines_data, &width, &cap](std::shared_ptr<Entity> entity
        ) -> Expected<std::shared_ptr<LinesVisual>, Error>
        {
            Expected<std::shared_ptr<GlLines>, Error> lines =
                entity->create_lines(lines_data);
            if (!lines)
                return Unexpected<Error>(Error());

            std::unique_ptr<LinesVisual::Impl> impl(
                std::make_unique<LinesVisual::Impl>(
                    entity, lines.value(), width, cap
                )
            );
            return std::shared_ptr<LinesVisual>(new LinesVisual(std::move(impl))
            );
        }
    );
}

LinesVisual::LinesVisual(LinesVisual &&other) : impl(std::move(other.impl)) {}
LinesVisual &LinesVisual::operator=(LinesVisual &&other)
{
    this->impl = std::move(other.impl);
    return *this;
}

LinesVisual::LinesVisual(LinesVisual &other) : impl(std::move(other.impl)) {}
LinesVisual &LinesVisual::operator=(LinesVisual &other)
{
    this->impl = std::move(other.impl);
    return *this;
}

void LinesVisual::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    this->impl->render(scene_size, depth_peeling_data);
}

void LinesVisual::set_model(const glm::mat4 &model)
{
    this->impl->model = model;
}

void LinesVisual::set_view(const glm::mat4 &view)
{
    this->impl->view = view;
}

void LinesVisual::set_projection(const glm::mat4 &projection)
{
    this->impl->projection = projection;
}

void LinesVisual::set_projection_aspect_correction(
    const bool projection_aspect_correction
)
{
    this->impl->projection_aspect_correction = projection_aspect_correction;
}

void LinesVisual::set_lines_data(const std::vector<Vertex> &lines_data)
{
    this->impl->set_lines_data(lines_data);
}

void LinesVisual::set_width(const float width)
{
    this->impl->width = width;
}

void LinesVisual::set_cap(const LineCap cap)
{
    this->impl->cap = cap;
}

LinesVisual::~LinesVisual() {}

LinesVisual::LinesVisual(std::unique_ptr<LinesVisual::Impl> impl)
    : impl(std::move(impl))
{}

SurfaceVisual::Impl::Impl(
    std::shared_ptr<Entity> entity, std::shared_ptr<GlSurface> surface
)
    : entity(entity),
      surface(surface),
      model(1.0f),
      view(1.0f),
      projection(1.0f),
      projection_aspect_correction(true),
      light_position(std::nullopt),
      ambient_color(0.25f, 0.25f, 0.25f),
      diffuse_color(0.5f, 0.5f, 0.5f),
      specular_color(0.5f, 0.5f, 0.5f),
      shininess(32.0f)
{}

void SurfaceVisual::Impl::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    std::shared_ptr<GlShaderProgram> shader_program =
        this->entity->surface_shader_program;
    shader_program->use();

    depth_peeling_set_uniforms(shader_program, depth_peeling_data);

    shader_program->set_uniform("scene_size", scene_size);

    shader_program->set_uniform("model", this->model);
    shader_program->set_uniform("view", this->view);
    shader_program->set_uniform(
        "projection",
        make_projection(
            this->projection, this->projection_aspect_correction, scene_size
        )
    );

    const glm::mat4 inverse_view = glm::affineInverse(view);
    glm::vec3 eye = glm::vec3(inverse_view[3]);
    shader_program->set_uniform("eye", eye);

    if (this->light_position)
        shader_program->set_uniform(
            "light_position", this->light_position.value()
        );
    else
    {
        shader_program->set_uniform("light_position", eye);
    }

    shader_program->set_uniform("ambient_color", this->ambient_color);
    shader_program->set_uniform("diffuse_color", this->diffuse_color);
    shader_program->set_uniform("specular_color", this->specular_color);
    shader_program->set_uniform("shininess", this->shininess);

    this->surface->render(false);
}

void SurfaceVisual::Impl::set_surface_data(const SurfaceData &surface_data)
{
    this->surface->set_surface_data(surface_data);
}

SurfaceVisual::Impl::~Impl(){};

Expected<std::shared_ptr<SurfaceVisual>, Error>
    SurfaceVisual::create(const SurfaceData &surface_data)
{
    return Entity::ensure_initialized_and_get().and_then(
        [&surface_data](std::shared_ptr<Entity> entity
        ) -> Expected<std::shared_ptr<SurfaceVisual>, Error>
        {
            Expected<std::shared_ptr<GlSurface>, Error> surface =
                entity->create_surface(surface_data);
            if (!surface)
                return Unexpected<Error>(Error());

            std::unique_ptr<SurfaceVisual::Impl> impl(
                std::make_unique<SurfaceVisual::Impl>(entity, surface.value())
            );
            return std::shared_ptr<SurfaceVisual>(
                new SurfaceVisual(std::move(impl))
            );
        }
    );
}

SurfaceVisual::SurfaceVisual(SurfaceVisual &&other)
    : impl(std::move(other.impl))
{}
SurfaceVisual &SurfaceVisual::operator=(SurfaceVisual &&other)
{
    this->impl = std::move(other.impl);
    return *this;
}

SurfaceVisual::SurfaceVisual(SurfaceVisual &other) : impl(std::move(other.impl))
{}
SurfaceVisual &SurfaceVisual::operator=(SurfaceVisual &other)
{
    this->impl = std::move(other.impl);
    return *this;
}

void SurfaceVisual::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    this->impl->render(scene_size, depth_peeling_data);
}

void SurfaceVisual::set_model(const glm::mat4 &model)
{
    this->impl->model = model;
}

void SurfaceVisual::set_view(const glm::mat4 &view)
{
    this->impl->view = view;
}

void SurfaceVisual::set_projection(const glm::mat4 &projection)
{
    this->impl->projection = projection;
}

void SurfaceVisual::set_projection_aspect_correction(
    const bool projection_aspect_correction
)
{
    this->impl->projection_aspect_correction = projection_aspect_correction;
}

void SurfaceVisual::set_surface_data(const SurfaceData &surface_data)
{
    this->impl->set_surface_data(surface_data);
}

void SurfaceVisual::set_light_position(
    const std::optional<glm::vec3> &light_position
)
{
    this->impl->light_position = light_position;
}

void SurfaceVisual::set_ambient_color(const glm::vec3 &ambient_color)
{
    this->impl->ambient_color = ambient_color;
}

void SurfaceVisual::set_diffuse_color(const glm::vec3 &diffuse_color)
{
    this->impl->diffuse_color = diffuse_color;
}

void SurfaceVisual::set_specular_color(const glm::vec3 &specular_color)
{
    this->impl->specular_color = specular_color;
}

void SurfaceVisual::set_shininess(const float shininess)
{
    this->impl->shininess = shininess;
}

SurfaceVisual::~SurfaceVisual() {}

SurfaceVisual::SurfaceVisual(std::unique_ptr<SurfaceVisual::Impl> impl)
    : impl(std::move(impl))
{}

CircleVisual::Impl::Impl(std::shared_ptr<Entity> entity, const glm::vec4 &color)
    : entity(entity),
      model(1.0f),
      view(1.0f),
      projection(1.0f),
      projection_aspect_correction(true),
      color(color)
{}

void CircleVisual::Impl::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    std::shared_ptr<GlShaderProgram> shader_program =
        this->entity->circle_shader_program;
    shader_program->use();

    depth_peeling_set_uniforms(shader_program, depth_peeling_data);

    shader_program->set_uniform("model", this->model);
    shader_program->set_uniform("view", this->view);
    shader_program->set_uniform(
        "projection",
        make_projection(
            this->projection, this->projection_aspect_correction, scene_size
        )
    );

    shader_program->set_uniform("scene_size", scene_size);

    shader_program->set_uniform("color", this->color);

    this->entity->circle->render(false);
}

CircleVisual::Impl::~Impl(){};

Expected<std::shared_ptr<CircleVisual>, Error>
    CircleVisual::create(const glm::vec4 &color)
{
    return Entity::ensure_initialized_and_get().and_then(
        [&color](std::shared_ptr<Entity> entity
        ) -> Expected<std::shared_ptr<CircleVisual>, Error>
        {
            std::unique_ptr<CircleVisual::Impl> impl(
                std::make_unique<CircleVisual::Impl>(entity, color)
            );
            return std::shared_ptr<CircleVisual>(new CircleVisual(std::move(impl
            )));
        }
    );
}

CircleVisual::CircleVisual(CircleVisual &&other) : impl(std::move(other.impl))
{}
CircleVisual &CircleVisual::operator=(CircleVisual &&other)
{
    this->impl = std::move(other.impl);
    return *this;
}

CircleVisual::CircleVisual(CircleVisual &other) : impl(std::move(other.impl)) {}
CircleVisual &CircleVisual::operator=(CircleVisual &other)
{
    this->impl = std::move(other.impl);
    return *this;
}

void CircleVisual::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    this->impl->render(scene_size, depth_peeling_data);
}

void CircleVisual::set_model(const glm::mat4 &model)
{
    this->impl->model = model;
}

void CircleVisual::set_view(const glm::mat4 &view)
{
    this->impl->view = view;
}

void CircleVisual::set_projection(const glm::mat4 &projection)
{
    this->impl->projection = projection;
}

void CircleVisual::set_projection_aspect_correction(
    const bool projection_aspect_correction
)
{
    this->impl->projection_aspect_correction = projection_aspect_correction;
}

void CircleVisual::set_color(const glm::vec4 &color)
{
    this->impl->color = color;
}

CircleVisual::~CircleVisual() {}

CircleVisual::CircleVisual(std::unique_ptr<CircleVisual::Impl> impl)
    : impl(std::move(impl))
{}

}
