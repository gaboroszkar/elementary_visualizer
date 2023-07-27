#ifndef ELEMENTARY_VISUALIZER_GLFW_RESOURCES_HPP
#define ELEMENTARY_VISUALIZER_GLFW_RESOURCES_HPP

#include <elementary_visualizer/elementary_visualizer.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>

namespace elementary_visualizer
{
class Glfw
{
public:

    static Expected<std::shared_ptr<Glfw>, Error> ensure_initialized_and_get();

    ~Glfw();

    Glfw(Glfw &&other) = delete;
    Glfw &operator=(Glfw &&other) = delete;
    Glfw(const Glfw &other) = delete;
    Glfw &operator=(const Glfw &other) = delete;

private:

    Glfw();
};

class WrappedGlfwWindow
{
public:

    static Expected<std::shared_ptr<WrappedGlfwWindow>, Error> create(
        const std::string &title,
        const glm::uvec2 &size,
        const bool resizable,
        std::shared_ptr<WrappedGlfwWindow> parent = nullptr,
        const bool visible = true
    );

    void make_current_context();
    void swap_buffers();
    glm::uvec2 get_framebuffer_size() const;
    glm::uvec2 get_window_size() const;
    int should_close() const;

    std::optional<std::function<void(int, int, int, int)>> key_callback;
    std::optional<std::function<void(int, int, int)>> mouse_button_callback;
    std::optional<std::function<void(double, double)>> cursor_pos_callback;

    ~WrappedGlfwWindow();

    WrappedGlfwWindow(WrappedGlfwWindow &&other) = delete;
    WrappedGlfwWindow &operator=(WrappedGlfwWindow &&other) = delete;
    WrappedGlfwWindow(const WrappedGlfwWindow &other) = delete;
    WrappedGlfwWindow &operator=(const WrappedGlfwWindow &other) = delete;

private:

    WrappedGlfwWindow(std::shared_ptr<Glfw> glfw, GLFWwindow *glfw_window);

    std::shared_ptr<Glfw> glfw;
    GLFWwindow *glfw_window;
};
}

#endif
