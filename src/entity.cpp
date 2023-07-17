#include <entity.hpp>

namespace elementary_visualizer
{
Expected<std::shared_ptr<Entity>, Error> Entity::ensure_initialized_and_get()
{
    static std::optional<Expected<std::shared_ptr<Entity>, Error>> entity =
        std::nullopt;
    if (!entity)
        entity = Entity::initialize();
    return entity.value();
}

Expected<std::shared_ptr<WrappedGlfwWindow>, Error> Entity::create_window(
    const std::string &title, const glm::ivec2 &size, const bool resizable
)
{
    return WrappedGlfwWindow::create(title, size, resizable, this->glfw_window);
}

Expected<std::shared_ptr<GlTexture>, Error>
    Entity::create_texture(const glm::ivec2 &size)
{
    return GlTexture::create(this->glfw_window, size);
}

Expected<std::shared_ptr<GlFramebufferTexture>, Error>
    Entity::create_framebuffer_texture(const glm::ivec2 &size)
{
    return GlFramebufferTexture::create(this->glfw_window, size);
}

void Entity::make_current_context()
{
    this->glfw_window->make_current_context();
}

Entity::~Entity() {}

Expected<std::shared_ptr<Entity>, Error> Entity::initialize()
{
    Expected<std::shared_ptr<WrappedGlfwWindow>, Error> window_creation_result =
        WrappedGlfwWindow::create(
            "Entity", glm::ivec2(1, 1), false, nullptr, false
        );
    return window_creation_result.and_then(
        [](std::shared_ptr<WrappedGlfwWindow> glfw_window
        ) -> Expected<std::shared_ptr<Entity>, Error>
        { return std::shared_ptr<Entity>(new Entity(glfw_window)); }
    );
}

Entity::Entity(std::shared_ptr<WrappedGlfwWindow> glfw_window)
    : glfw_window(glfw_window)
{}
}
