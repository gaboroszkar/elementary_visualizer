#ifndef ELEMENTARY_VISUALIZER_ENTITY_HPP
#define ELEMENTARY_VISUALIZER_ENTITY_HPP

#include <elementary_visualizer/elementary_visualizer.hpp>
#include <gl_resources.hpp>
#include <glfw_resources.hpp>
#include <memory>
#include <optional>
#include <string>

namespace elementary_visualizer
{
class Entity
{
public:

    static Expected<std::shared_ptr<Entity>, Error>
        ensure_initialized_and_get();

    Expected<std::shared_ptr<WrappedGlfwWindow>, Error> create_window(
        const std::string &title, const glm::ivec2 &size, const bool resizable
    );
    Expected<std::shared_ptr<GlTexture>, Error>
        create_texture(const glm::ivec2 &size);
    Expected<std::shared_ptr<GlFramebufferTexture>, Error>
        create_framebuffer_texture(const glm::ivec2 &size);

    void make_current_context();

    ~Entity();

    Entity(Entity &&other) = delete;
    Entity &operator=(Entity &&other) = delete;
    Entity(const Entity &other) = delete;
    Entity &operator=(const Entity &other) = delete;

private:

    static Expected<std::shared_ptr<Entity>, Error> initialize();

    Entity(std::shared_ptr<WrappedGlfwWindow> glfw_window);

    std::shared_ptr<WrappedGlfwWindow> glfw_window;
};
}

#endif
