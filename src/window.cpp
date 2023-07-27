#include <gl_shader_program.hpp>
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <window.hpp>

namespace elementary_visualizer
{
Window::Impl::Impl(
    std::shared_ptr<Entity> entity,
    std::shared_ptr<WrappedGlfwWindow> glfw_window,
    std::shared_ptr<GlQuad> quad
)
    : entity(entity), glfw_window(glfw_window), quad(quad)
{}

Window::Impl::operator bool() const
{
    return static_cast<bool>(this->glfw_window);
}

bool Window::Impl::should_close_or_invalid() const
{
    if (!this->glfw_window)
        return true;
    return this->glfw_window->should_close() == GLFW_TRUE;
}

void Window::Impl::render(
    std::shared_ptr<const GlTexture> rendered_scene,
    const RenderMode render_mode
)
{
    if (!rendered_scene)
        return;

    if (this->glfw_window)
    {
        glDisable(GL_DEPTH_TEST);

        const glm::uvec2 window_size =
            this->glfw_window->get_framebuffer_size();
        const glm::uvec2 scene_size = rendered_scene->get_size();

        this->glfw_window->make_current_context();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_size.x, window_size.y);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        std::shared_ptr<GlShaderProgram> shader_program =
            this->entity->quad_shader_program;
        shader_program->use(false);

        const float window_width = static_cast<float>(window_size.x);
        const float window_height = static_cast<float>(window_size.y);
        const float scene_width = static_cast<float>(scene_size.x);
        const float scene_height = static_cast<float>(scene_size.y);
        const float window_aspect = window_width / window_height;
        const float scene_aspect = scene_width / scene_height;

        glm::vec3 scale = glm::vec3(scene_aspect, 1.0f, 1.0f);
        glm::mat4 model = glm::scale(glm::mat4(1.0f), scale);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);
        if ((render_mode == RenderMode::fill && window_aspect > scene_aspect) ||
            (render_mode == RenderMode::fit && window_aspect <= scene_aspect))
        {
            projection = glm::ortho(
                -scene_aspect,
                +scene_aspect,
                -scene_aspect / window_aspect,
                +scene_aspect / window_aspect
            );
        }
        else if ((render_mode == RenderMode::fill && window_aspect <= scene_aspect) || (render_mode == RenderMode::fit && window_aspect > scene_aspect))
        {
            projection =
                glm::ortho(-window_aspect, +window_aspect, -1.0f, +1.0f);
        }
        else if (render_mode == RenderMode::absolute)
        {
            projection = glm::ortho(
                -window_width / scene_height,
                +window_width / scene_height,
                -window_height / scene_height,
                +window_height / scene_height
            );
        }

        shader_program->set_uniform("model", model);
        shader_program->set_uniform("view", view);
        shader_program->set_uniform("projection", projection);

        const int texture_slot = 0;
        glActiveTexture(GL_TEXTURE0 + texture_slot);
        rendered_scene->bind(false);
        shader_program->set_uniform("texture_slot", texture_slot);

        this->quad->render();

        this->glfw_window->swap_buffers();
    }
}

void Window::Impl::destroy()
{
    if (this->glfw_window)
        this->glfw_window.reset();
}

void Window::Impl::on_keyboard_event(
    std::optional<std::function<void(EventAction, Key, ModifierKey)>> function
)
{
    if (this->glfw_window)
    {
        this->glfw_window->key_callback =
            [function](int key, int, int action, int mods) -> void
        {
            if (function)
                function.value(
                )(static_cast<EventAction>(action),
                  static_cast<Key>(key),
                  static_cast<ModifierKey>(mods));
        };
    }
}

void Window::Impl::on_mouse_button_event(
    std::optional<std::function<void(EventAction, MouseButton, ModifierKey)>>
        function
)
{
    if (this->glfw_window)
    {
        this->glfw_window->mouse_button_callback =
            [function](int button, int action, int mods) -> void
        {
            if (function)
                function.value(
                )(static_cast<EventAction>(action),
                  static_cast<MouseButton>(button),
                  static_cast<ModifierKey>(mods));
        };
    }
}

void Window::Impl::on_mouse_move_event(
    std::optional<std::function<void(glm::vec2)>> function
)
{
    if (this->glfw_window)
    {
        this->glfw_window->cursor_pos_callback =
            [function](double xpos, double ypos) -> void
        {
            if (function)
                function.value()(glm::vec2(xpos, ypos));
        };
    }
}

glm::uvec2 Window::Impl::get_size() const
{
    return this->glfw_window->get_window_size();
}

Window::Impl::~Impl(){};

Expected<Window, Error> Window::create(
    const std::string &title, const glm::uvec2 &size, const bool resizable
)
{
    Expected<std::shared_ptr<Entity>, Error> entity =
        Entity::ensure_initialized_and_get();
    if (!entity)
        return Unexpected<Error>(Error());

    Expected<std::shared_ptr<WrappedGlfwWindow>, Error> glfw_window(
        entity.value()->create_window(title, size, resizable)
    );
    if (!glfw_window)
        return Unexpected<Error>(Error());

    Expected<std::shared_ptr<GlQuad>, Error> quad(
        GlQuad::create(glfw_window.value())
    );
    if (!quad)
        return Unexpected<Error>(Error());

    return Window(std::make_unique<Impl>(
        entity.value(), glfw_window.value(), quad.value()
    ));
}

Window::Window(Window &&other) : impl(std::move(other.impl)) {}

Window &Window::operator=(Window &&other)
{
    this->impl = std::move(other.impl);
    return *this;
}

Window::operator bool() const
{
    return static_cast<bool>(this->impl);
}

void Window::destroy()
{
    impl->destroy();
}

bool Window::should_close_or_invalid() const
{
    return impl->should_close_or_invalid();
}

void Window::render(
    std::shared_ptr<const GlTexture> rendered_scene,
    const RenderMode render_mode
)
{
    this->impl->render(rendered_scene, render_mode);
}

void Window::on_keyboard_event(
    std::optional<std::function<void(EventAction, Key, ModifierKey)>> function
)
{
    this->impl->on_keyboard_event(function);
}

void Window::on_mouse_button_event(
    std::optional<std::function<void(EventAction, MouseButton, ModifierKey)>>
        function
)
{
    this->impl->on_mouse_button_event(function);
}

void Window::on_mouse_move_event(
    std::optional<std::function<void(glm::vec2)>> function
)
{
    this->impl->on_mouse_move_event(function);
}

glm::uvec2 Window::get_size() const
{
    return this->impl->get_size();
}

Window::~Window() {}

Window::Window(std::unique_ptr<Window::Impl> &&impl) : impl(std::move(impl)) {}
}
