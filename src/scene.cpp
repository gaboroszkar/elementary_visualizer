#include <glad/gl.h>
#include <scene.hpp>

namespace elementary_visualizer
{
void depth_peeling_set_uniforms(
    std::shared_ptr<GlShaderProgram> shader_program,
    const DepthPeelingData &depth_peeling_data
)
{
    if (!shader_program || !depth_peeling_data.depth_texture)
        return;

    shader_program->set_uniform(
        "depth_peeling_first_pass", depth_peeling_data.first_pass
    );
    const bool multisampled =
        depth_peeling_data.depth_texture->samples.has_value();
    shader_program->set_uniform("depth_peeling_multisampled", multisampled);

    if (!multisampled)
        shader_program->set_uniform(
            "scene_size", depth_peeling_data.depth_texture->get_size()
        );

    // We need to set both textures in order for the fragment shader to work,
    // even if we use only one of them. Unfortunately the shader will
    // fail to work if only one of them is set.
    {
        const int texture_slot = 0;
        glActiveTexture(GL_TEXTURE0 + texture_slot);
        depth_peeling_data.depth_texture->bind(false);
        shader_program->set_uniform("depth_peeling_texture_slot", texture_slot);
    }
    {
        const int texture_slot = 1;
        glActiveTexture(GL_TEXTURE0 + texture_slot);
        depth_peeling_data.depth_texture->bind(false);
        shader_program->set_uniform(
            "depth_peeling_texture_slot_multisampled", texture_slot
        );
    }
}

Scene::Impl::Impl(
    std::shared_ptr<Entity> entity,
    std::shared_ptr<GlFramebufferTexture> framebuffer_texture,
    std::shared_ptr<GlFramebufferTexture>
        framebuffer_texture_possibly_multisampled,
    std::array<std::shared_ptr<GlTexture>, 2> depth_textures,
    std::vector<std::shared_ptr<GlFramebufferTexture>>
        depth_peeling_render_textures,
    const glm::vec4 &background_color
)
    : entity(entity),
      framebuffer_texture(framebuffer_texture),
      framebuffer_texture_possibly_multisampled(
          framebuffer_texture_possibly_multisampled
      ),
      depth_textures(depth_textures),
      depth_peeling_render_textures(depth_peeling_render_textures),
      background_color(background_color)
{}

void Scene::Impl::add_visual(std::shared_ptr<Visual> visual)
{
    this->visuals.insert(visual);
}

void Scene::Impl::remove_visual(std::shared_ptr<Visual> visual)
{
    this->visuals.erase(visual);
}

std::shared_ptr<const GlTexture> Scene::Impl::render()
{
    this->entity->make_current_context();

    const glm::uvec2 scene_size =
        this->framebuffer_texture_possibly_multisampled->texture->get_size();

    // We implement here the depth peeling method. See
    // <https://en.wikipedia.org/wiki/Depth_peeling>,
    // Interactive Order-Independent Transparency, Cass Everitt,
    // <https://my.eng.utah.edu/~cs5610/handouts/order_independent_transparency.pdf>,
    // <https://stackoverflow.com/q/43364887>.

    this->framebuffer_texture_possibly_multisampled->framebuffer->bind(false);

    // Initialize depth textures.
    // This depth texture is the already "peeled" texture from the last pass.
    // In each pass, the rendered fragments cannot be nearer than this depth.
    // Initially it is set to the nearest depth possible, to 0.
    std::shared_ptr<GlTexture> peeled_depth_texture = this->depth_textures[0];

    // This depth texture is the regular depth texture.
    // In each pass, the rendered fragments will find the nearest (lowest
    // value). This behaves like a regular depth buffer.
    std::shared_ptr<GlTexture> regular_depth_texture = this->depth_textures[1];

    // Front to back rendering, while peeling away each front layers.
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // We render each pass in a different texture, from front to back.
    // Each of these depth peeling passes will be rendered onto a different
    // texture.
    bool first_pass = true;
    for (auto it = std::begin(this->depth_peeling_render_textures);
         it != std::end(this->depth_peeling_render_textures);
         ++it)
    {
        // Setup the rendering texture and depth texture for the depth peeling
        // pass.
        (*it)->texture->bind(false);
        (*it)->texture->framebuffer_texture(false);
        regular_depth_texture->bind(false);
        regular_depth_texture->framebuffer_texture(false);

        glViewport(0, 0, scene_size.x, scene_size.y);

        // Clear rendering texture and depth texture.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render visuals.
        for (const auto &visual : this->visuals)
            visual->render(
                scene_size, DepthPeelingData(first_pass, peeled_depth_texture)
            );
        glFinish();

        // Swap the peeled and regular depth texture, so that in the next pass
        // the regular depth texture becomes the already peeled away depth.
        std::swap(peeled_depth_texture, regular_depth_texture);
        first_pass = false;
    }

    // Now, we render each depth peeled pass from the textures we rendered in
    // the previous loop, from back to front to a scene quad, but now, with
    // proper alpha blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    this->framebuffer_texture_possibly_multisampled->texture->bind(false);
    this->framebuffer_texture_possibly_multisampled->texture
        ->framebuffer_texture(false);

    glViewport(0, 0, scene_size.x, scene_size.y);

    glClearColor(
        this->background_color.r,
        this->background_color.g,
        this->background_color.b,
        this->background_color.a
    );
    glClear(GL_COLOR_BUFFER_BIT);

    const bool multisampled = this->framebuffer_texture_possibly_multisampled
                                  ->texture->samples.has_value();
    std::shared_ptr<GlShaderProgram> shader_program =
        multisampled ? this->entity->quad_multisampled_shader_program
                     : this->entity->quad_shader_program;
    shader_program->use(false);

    shader_program->set_uniform("model", glm::mat4(1.0f));
    shader_program->set_uniform("view", glm::mat4(1.0f));
    shader_program->set_uniform("projection", glm::mat4(1.0f));

    for (auto it = std::rbegin(this->depth_peeling_render_textures);
         it != std::rend(this->depth_peeling_render_textures);
         ++it)
    {
        if (multisampled)
            shader_program->set_uniform("scene_size", scene_size);

        const int texture_slot = 0;
        glActiveTexture(GL_TEXTURE0 + texture_slot);
        (*it)->texture->bind(false);
        shader_program->set_uniform("texture_slot", texture_slot);

        this->entity->quad->render();
    }

    // Waiting until the rendering queue is finished,
    // so that we will return a rendered texture.
    glFinish();

    // We convert the multisampled texture to non-multisampled texture, and
    // return with that.
    this->framebuffer_texture_possibly_multisampled->framebuffer->bind(
        false, FrameBufferBindType::read
    );
    this->framebuffer_texture->framebuffer->bind(
        false, FrameBufferBindType::draw
    );
    glBlitFramebuffer(
        0,
        0,
        scene_size.x,
        scene_size.y,
        0,
        0,
        scene_size.x,
        scene_size.y,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR
    );

    // Waiting until the rendering queue is finished,
    // so that we will return a rendered texture.
    glFinish();

    return this->framebuffer_texture->texture;
}

Scene::Impl::~Impl(){};

Expected<std::shared_ptr<Scene>, Error> Scene::create(
    const glm::uvec2 &size,
    const glm::vec4 &background_color,
    const std::optional<int> samples,
    const int depth_peeling_passes
)
{
    return Entity::ensure_initialized_and_get().and_then(
        [&size, &background_color, &samples, &depth_peeling_passes](
            std::shared_ptr<Entity> entity
        ) -> Expected<std::shared_ptr<Scene>, Error>
        {
            Expected<std::shared_ptr<GlFramebufferTexture>, Error>
                framebuffer_texture =
                    entity->create_framebuffer_texture(size, std::nullopt);
            if (!framebuffer_texture)
                return Unexpected<Error>(Error());

            Expected<std::shared_ptr<GlFramebufferTexture>, Error>
                framebuffer_texture_possibly_multisampled =
                    entity->create_framebuffer_texture(size, samples);
            if (!framebuffer_texture_possibly_multisampled)
                return Unexpected<Error>(Error());

            Expected<std::shared_ptr<GlTexture>, Error> depth_texture_0 =
                entity->create_texture(size, true, samples);
            if (!depth_texture_0)
                return Unexpected<Error>(Error());

            Expected<std::shared_ptr<GlTexture>, Error> depth_texture_1 =
                entity->create_texture(size, true, samples);
            if (!depth_texture_1)
                return Unexpected<Error>(Error());

            std::vector<std::shared_ptr<GlFramebufferTexture>>
                depth_peeling_render_textures;
            for (int i = 0; i < depth_peeling_passes; ++i)
            {
                Expected<std::shared_ptr<GlFramebufferTexture>, Error>
                    render_texture =
                        entity->create_framebuffer_texture(size, samples);
                if (!render_texture)
                    return Unexpected<Error>(Error());

                depth_peeling_render_textures.push_back(render_texture.value());
            }

            std::unique_ptr<Scene::Impl> impl(std::make_unique<Impl>(
                entity,
                framebuffer_texture.value(),
                framebuffer_texture_possibly_multisampled.value(),
                std::array<std::shared_ptr<GlTexture>, 2>(
                    {depth_texture_0.value(), depth_texture_1.value()}
                ),
                depth_peeling_render_textures,
                background_color
            ));

            return std::shared_ptr<Scene>(new Scene(std::move(impl)));
        }
    );
}

Scene::Scene(Scene &&other) : impl(std::move(other.impl)) {}

Scene &Scene::operator=(Scene &&other)
{
    this->impl = std::move(other.impl);
    return *this;
}

void Scene::set_background_color(const glm::vec4 &color)
{
    this->impl->background_color = color;
}

Expected<glm::vec4, Error> Scene::get_background_color() const
{
    return this->impl->background_color;
}

void Scene::add_visual(std::shared_ptr<Visual> visual)
{
    this->impl->add_visual(visual);
}

void Scene::remove_visual(std::shared_ptr<Visual> visual)
{
    this->impl->remove_visual(visual);
}

std::shared_ptr<const RenderedScene> Scene::render()
{
    return this->impl->render();
}

Scene::~Scene() {}

Scene::Scene(std::unique_ptr<Scene::Impl> &&impl) : impl(std::move(impl)) {}
}
