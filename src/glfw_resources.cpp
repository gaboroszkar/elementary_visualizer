#include <glad/gl.h>
#include <glfw_resources.hpp>

namespace elementary_visualizer
{
Expected<std::shared_ptr<Glfw>, Error> Glfw::ensure_initialized_and_get()
{
    static std::optional<Expected<std::shared_ptr<Glfw>, Error>> glfw =
        std::nullopt;
    if (!glfw)
    {
        const int glfw_init_result = glfwInit();
        if (glfw_init_result != GLFW_TRUE)
            return Unexpected<Error>(Error());
        glfw = std::shared_ptr<Glfw>(new Glfw());
    }
    return glfw.value();
}

Glfw::~Glfw()
{
    glfwTerminate();
}

Glfw::Glfw() {}

Expected<std::shared_ptr<WrappedGlfwWindow>, Error> WrappedGlfwWindow::create(
    const std::string &title,
    const glm::ivec2 &size,
    const bool resizable,
    std::shared_ptr<WrappedGlfwWindow> parent,
    const bool visible
)
{
    return Glfw::ensure_initialized_and_get().and_then(
        [&title, &size, &resizable, &parent, &visible](
            std::shared_ptr<Glfw> glfw
        ) -> Expected<std::shared_ptr<WrappedGlfwWindow>, Error>
        {
            // Tell GLFW which version of OpenGL we are using.
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);

            // Tell GLFW we are using the CORE profile.
            // That means we only have the modern functions.
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            // Set whether the window is visible.
            glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);

            // Set whether the window is visible.
            glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

            GLFWwindow *parent_glfw_window =
                parent ? parent->glfw_window : nullptr;
            GLFWwindow *glfw_window = glfwCreateWindow(
                size.x, size.y, title.c_str(), nullptr, parent_glfw_window
            );

            std::shared_ptr<WrappedGlfwWindow> window =
                std::shared_ptr<WrappedGlfwWindow>(
                    new WrappedGlfwWindow(glfw, glfw_window)
                );

            glfwSetWindowUserPointer(glfw_window, window.get());
            glfwSetKeyCallback(
                glfw_window,
                [](GLFWwindow *glfw_window,
                   int key,
                   int scancode,
                   int action,
                   int mods)
                {
                    WrappedGlfwWindow *window =
                        static_cast<WrappedGlfwWindow *>(
                            glfwGetWindowUserPointer(glfw_window)
                        );
                    if (window && window->key_callback)
                        window->key_callback.value(
                        )(key, scancode, action, mods);
                }
            );
            glfwSetMouseButtonCallback(
                glfw_window,
                [](GLFWwindow *glfw_window, int button, int action, int mods)
                {
                    WrappedGlfwWindow *window =
                        static_cast<WrappedGlfwWindow *>(
                            glfwGetWindowUserPointer(glfw_window)
                        );
                    if (window && window->mouse_button_callback)
                        window->mouse_button_callback.value(
                        )(button, action, mods);
                }
            );
            glfwSetCursorPosCallback(
                glfw_window,
                [](GLFWwindow *glfw_window, double xpos, double ypos)
                {
                    WrappedGlfwWindow *window =
                        static_cast<WrappedGlfwWindow *>(
                            glfwGetWindowUserPointer(glfw_window)
                        );
                    if (window && window->cursor_pos_callback)
                        window->cursor_pos_callback.value()(xpos, ypos);
                }
            );

            if (!parent)
            {
                // Introduce the window into the current context.
                glfwMakeContextCurrent(glfw_window);
                // Load glad so it configures OpenGL.
                gladLoadGL(glfwGetProcAddress);
            }

            return window;
        }
    );
}

void WrappedGlfwWindow::make_current_context()
{
    if (glfwGetCurrentContext() != this->glfw_window)
        glfwMakeContextCurrent(this->glfw_window);
}

void WrappedGlfwWindow::swap_buffers()
{
    this->make_current_context();
    glfwSwapBuffers(this->glfw_window);
}

glm::ivec2 WrappedGlfwWindow::get_framebuffer_size() const
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(this->glfw_window, &width, &height);
    return glm::ivec2(width, height);
}

int WrappedGlfwWindow::should_close() const
{
    return glfwWindowShouldClose(this->glfw_window);
    return GLFW_TRUE;
}

WrappedGlfwWindow::~WrappedGlfwWindow()
{
    glfwDestroyWindow(this->glfw_window);
}

WrappedGlfwWindow::WrappedGlfwWindow(
    std::shared_ptr<Glfw> glfw, GLFWwindow *glfw_window
)
    : glfw(glfw), glfw_window(glfw_window)
{}
}
