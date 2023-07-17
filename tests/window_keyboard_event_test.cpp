#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>

namespace ev = elementary_visualizer;

void emulate_key_event(
    GLFWwindow *glfw_window, int key, int scancode, int action, int mods
);

int main(int, char **)
{
    const glm::ivec2 scene_size_0(1280, 720);
    auto window_0 = ev::Window::create("Window 0", scene_size_0, false);
    if (!window_0)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window_0 = glfwGetCurrentContext();

    const glm::ivec2 scene_size_1(500, 500);
    auto window_1 = ev::Window::create("Window 1", scene_size_1, true);
    if (!window_1)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window_1 = glfwGetCurrentContext();

    ev::EventAction key_action_0;
    ev::Key key_state_0;
    ev::ModifierKey modifier_key_state_0;
    window_0->on_keyboard_event(
        [&](const ev::EventAction action,
            const ev::Key key,
            const ev::ModifierKey modifier_key)
        {
            key_action_0 = action;
            key_state_0 = key;
            modifier_key_state_0 = modifier_key;
        }
    );

    ev::EventAction key_action_1;
    ev::Key key_state_1;
    ev::ModifierKey modifier_key_state_1;
    window_1->on_keyboard_event(
        [&](const ev::EventAction action,
            const ev::Key key,
            const ev::ModifierKey modifier_key)
        {
            key_action_1 = action;
            key_state_1 = key;
            modifier_key_state_1 = modifier_key;
        }
    );

    emulate_key_event(glfw_window_0, GLFW_KEY_ENTER, 0, GLFW_PRESS, 0);
    if (key_action_0 != ev::EventAction::press ||
        key_state_0 != ev::Key::enter ||
        modifier_key_state_0 != ev::ModifierKey::none)
        return EXIT_FAILURE;

    emulate_key_event(glfw_window_1, GLFW_KEY_BACKSPACE, 0, GLFW_PRESS, 0);
    if (key_action_1 != ev::EventAction::press ||
        key_state_1 != ev::Key::backspace ||
        modifier_key_state_1 != ev::ModifierKey::none)
        return EXIT_FAILURE;

    emulate_key_event(glfw_window_0, GLFW_KEY_ENTER, 0, GLFW_RELEASE, 0);
    if (key_action_0 != ev::EventAction::release ||
        key_state_0 != ev::Key::enter ||
        modifier_key_state_0 != ev::ModifierKey::none)
        return EXIT_FAILURE;

    emulate_key_event(glfw_window_1, GLFW_KEY_BACKSPACE, 0, GLFW_RELEASE, 0);
    if (key_action_1 != ev::EventAction::release ||
        key_state_1 != ev::Key::backspace ||
        modifier_key_state_1 != ev::ModifierKey::none)
        return EXIT_FAILURE;

    emulate_key_event(
        glfw_window_0, GLFW_KEY_SPACE, 0, GLFW_PRESS, GLFW_MOD_SHIFT
    );
    if (key_action_0 != ev::EventAction::press ||
        key_state_0 != ev::Key::space ||
        modifier_key_state_0 != ev::ModifierKey::shift)
        return EXIT_FAILURE;

    emulate_key_event(
        glfw_window_1, GLFW_KEY_ESCAPE, 0, GLFW_PRESS, GLFW_MOD_CONTROL
    );
    if (key_action_1 != ev::EventAction::press ||
        key_state_1 != ev::Key::escape ||
        modifier_key_state_1 != ev::ModifierKey::control)
        return EXIT_FAILURE;

    emulate_key_event(
        glfw_window_0, GLFW_KEY_SPACE, 0, GLFW_RELEASE, GLFW_MOD_SHIFT
    );
    if (key_action_0 != ev::EventAction::release ||
        key_state_0 != ev::Key::space ||
        modifier_key_state_0 != ev::ModifierKey::shift)
        return EXIT_FAILURE;

    emulate_key_event(
        glfw_window_1, GLFW_KEY_ESCAPE, 0, GLFW_RELEASE, GLFW_MOD_CONTROL
    );
    if (key_action_1 != ev::EventAction::release ||
        key_state_1 != ev::Key::escape ||
        modifier_key_state_1 != ev::ModifierKey::control)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

GLFWkeyfun glfw_get_key_callback(GLFWwindow *window)
{
    GLFWkeyfun callback = glfwSetKeyCallback(window, nullptr);
    glfwSetKeyCallback(window, callback);
    return callback;
}

void emulate_key_event(
    GLFWwindow *glfw_window, int key, int scancode, int action, int mods
)
{
    glfw_get_key_callback(glfw_window
    )(glfw_window, key, scancode, action, mods);
}
