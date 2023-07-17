#include <glad/gl.h>
#include <scene.hpp>

namespace elementary_visualizer
{
Scene::Impl::Impl(
    std::shared_ptr<Entity> entity,
    std::shared_ptr<GlFramebufferTexture> framebuffer_texture,
    const glm::vec4 &background_color
)
    : entity(entity),
      framebuffer_texture(framebuffer_texture),
      background_color(background_color)
{}

Scene::Impl::Impl(Scene::Impl &&other)
    : entity(std::move(other.entity)),
      framebuffer_texture(std::move(other.framebuffer_texture)),
      background_color(other.background_color)
{}

Scene::Impl &Scene::Impl::operator=(Scene::Impl &&other)
{
    this->entity = std::move(other.entity);
    this->framebuffer_texture = std::move(other.framebuffer_texture);
    this->background_color = other.background_color;
    return *this;
}

std::shared_ptr<const GlTexture> Scene::Impl::render()
{
    this->entity->make_current_context();

    const glm::ivec2 scene_size =
        this->framebuffer_texture->texture->get_size();

    this->framebuffer_texture->framebuffer->bind(false);
    this->framebuffer_texture->texture->framebuffer_texture(false);

    glViewport(0, 0, scene_size.x, scene_size.y);

    glClearColor(
        this->background_color.r,
        this->background_color.g,
        this->background_color.b,
        this->background_color.a
    );
    glClear(GL_COLOR_BUFFER_BIT);

    // Waiting until the rendering queue is finished,
    // so that we will return a rendered texture.
    glFinish();

    return this->framebuffer_texture->texture;
}

Scene::Impl::~Impl(){};

Expected<Scene, Error>
    Scene::create(const glm::ivec2 &size, const glm::vec4 &background_color)
{
    return Entity::ensure_initialized_and_get().and_then(
        [&size, &background_color](std::shared_ptr<Entity> entity
        ) -> Expected<Scene, Error>
        {
            Expected<std::shared_ptr<GlFramebufferTexture>, Error>
                framebuffer_texture = entity->create_framebuffer_texture(size);
            if (!framebuffer_texture)
                return Unexpected<Error>(Error());

            return Scene(std::make_unique<Impl>(
                entity, framebuffer_texture.value(), background_color
            ));
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

std::shared_ptr<const RenderedScene> Scene::render()
{
    return this->impl->render();
}

Scene::~Scene() {}

Scene::Scene(std::unique_ptr<Scene::Impl> &&impl) : impl(std::move(impl)) {}
}
