#include <scene.hpp>
#include <visuals.hpp>

namespace elementary_visualizer
{
LinesegmentsVisual::Impl::Impl(
    std::shared_ptr<Entity> entity, std::shared_ptr<GlLinesegments> linesegments
)
    : entity(entity),
      linesegments(linesegments),
      model(1.0f),
      view(1.0f),
      projection(1.0f)
{}

LinesegmentsVisual::Impl::Impl(LinesegmentsVisual::Impl &&other)
    : entity(other.entity),
      linesegments(other.linesegments),
      model(other.model),
      view(other.view),
      projection(other.projection)
{}
LinesegmentsVisual::Impl &
    LinesegmentsVisual::Impl::operator=(LinesegmentsVisual::Impl &&other)
{
    this->entity = other.entity;
    this->linesegments = other.linesegments;
    this->model = other.model;
    this->view = other.view;
    this->projection = other.projection;
    return *this;
}

void LinesegmentsVisual::Impl::render(
    const glm::ivec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    std::shared_ptr<GlShaderProgram> shader_program =
        this->entity->linesegments_shader_program;
    shader_program->use();

    depth_peeling_set_uniforms(shader_program, depth_peeling_data);

    shader_program->set_uniform("model", this->model);
    shader_program->set_uniform("view", this->view);

    glm::mat4 projection = this->projection;
    const float aspect =
        static_cast<float>(scene_size.x) / static_cast<float>(scene_size.y);
    projection[0][0] = projection[0][0] / aspect;
    shader_program->set_uniform("projection", projection);

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

Expected<std::shared_ptr<LinesegmentsVisual>, Error>
    LinesegmentsVisual::create(const std::vector<Linesegment> &linesegments_data
    )
{
    return Entity::ensure_initialized_and_get().and_then(
        [&linesegments_data](std::shared_ptr<Entity> entity
        ) -> Expected<std::shared_ptr<LinesegmentsVisual>, Error>
        {
            Expected<std::shared_ptr<GlLinesegments>, Error> linesegments =
                entity->create_linesegments(linesegments_data);
            if (!linesegments)
                return Unexpected<Error>(Error());

            std::unique_ptr<LinesegmentsVisual::Impl> impl(
                std::make_unique<LinesegmentsVisual::Impl>(
                    entity, linesegments.value()
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
    const glm::ivec2 &scene_size, const DepthPeelingData &depth_peeling_data
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

void LinesegmentsVisual::set_linesegments_data(
    const std::vector<Linesegment> &linesegments_data
)
{
    this->impl->set_linesegments_data(linesegments_data);
}

LinesegmentsVisual::~LinesegmentsVisual() {}

LinesegmentsVisual::LinesegmentsVisual(
    std::unique_ptr<LinesegmentsVisual::Impl> impl
)
    : impl(std::move(impl))
{}
}
