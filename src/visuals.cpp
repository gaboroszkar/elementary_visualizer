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
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
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

LinesVisual::Impl::Impl(
    std::shared_ptr<Entity> entity,
    std::shared_ptr<GlLines> lines,
    const float width
)
    : entity(entity),
      lines(lines),
      width(width),
      model(1.0f),
      view(1.0f),
      projection(1.0f)
{}

LinesVisual::Impl::Impl(LinesVisual::Impl &&other)
    : entity(other.entity),
      lines(other.lines),
      width(other.width),
      model(other.model),
      view(other.view),
      projection(other.projection)
{}
LinesVisual::Impl &LinesVisual::Impl::operator=(LinesVisual::Impl &&other)
{
    this->entity = other.entity;
    this->lines = other.lines;
    this->width = other.width;
    this->model = other.model;
    this->view = other.view;
    this->projection = other.projection;
    return *this;
}

void LinesVisual::Impl::render(
    const glm::uvec2 &scene_size, const DepthPeelingData &depth_peeling_data
) const
{
    std::shared_ptr<GlShaderProgram> shader_program =
        this->entity->lines_shader_program;
    shader_program->use();

    depth_peeling_set_uniforms(shader_program, depth_peeling_data);

    shader_program->set_uniform("line_width", this->width);

    shader_program->set_uniform("model", this->model);
    shader_program->set_uniform("view", this->view);

    glm::mat4 projection = this->projection;
    const float aspect =
        static_cast<float>(scene_size.x) / static_cast<float>(scene_size.y);
    projection[0][0] = projection[0][0] / aspect;
    shader_program->set_uniform("projection", projection);

    shader_program->set_uniform("scene_size", scene_size);

    this->lines->render(false);
}

void LinesVisual::Impl::set_lines_data(const std::vector<Vertex> &lines_data)
{
    this->lines->set_lines_data(lines_data);
}

LinesVisual::Impl::~Impl(){};

Expected<std::shared_ptr<LinesVisual>, Error> LinesVisual::create(
    const std::vector<Vertex> &lines_data, const float width
)
{
    return Entity::ensure_initialized_and_get().and_then(
        [&lines_data, &width](std::shared_ptr<Entity> entity
        ) -> Expected<std::shared_ptr<LinesVisual>, Error>
        {
            Expected<std::shared_ptr<GlLines>, Error> lines =
                entity->create_lines(lines_data);
            if (!lines)
                return Unexpected<Error>(Error());

            std::unique_ptr<LinesVisual::Impl> impl(
                std::make_unique<LinesVisual::Impl>(
                    entity, lines.value(), width
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

void LinesVisual::set_lines_data(const std::vector<Vertex> &lines_data)
{
    this->impl->set_lines_data(lines_data);
}

void LinesVisual::set_width(const float width)
{
    this->impl->width = width;
}

LinesVisual::~LinesVisual() {}

LinesVisual::LinesVisual(std::unique_ptr<LinesVisual::Impl> impl)
    : impl(std::move(impl))
{}
}
