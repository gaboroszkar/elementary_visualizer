#include <cstdlib>
#include <elementary_visualizer/elementary_visualizer.hpp>

namespace ev = elementary_visualizer;

void emulate_mouse_button_event(
    GLFWwindow *glfw_window, int button, int action, int mods
);
void emulate_mouse_move_event(
    GLFWwindow *glfw_window, double xpos, double ypos
);

int main(int, char **)
{
    const glm::uvec2 scene_size_0(1280, 720);
    auto window_0 = ev::Window::create("Window 0", scene_size_0, false);
    if (!window_0)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window_0 = glfwGetCurrentContext();

    const glm::uvec2 scene_size_1(500, 500);
    auto window_1 = ev::Window::create("Window 1", scene_size_1, true);
    if (!window_1)
        return EXIT_FAILURE;
    GLFWwindow *glfw_window_1 = glfwGetCurrentContext();

    ev::EventAction button_action_0;
    ev::MouseButton button_state_0;
    ev::ModifierKey modifier_key_state_0;
    window_0.value()->on_mouse_button_event(
        [&](const ev::EventAction action,
            const ev::MouseButton button,
            const ev::ModifierKey modifier_key)
        {
            button_action_0 = action;
            button_state_0 = button;
            modifier_key_state_0 = modifier_key;
        }
    );
    glm::vec2 mouse_pos_0;
    window_0.value()->on_mouse_move_event([&](const glm::vec2 mouse_pos)
                                          { mouse_pos_0 = mouse_pos; });

    ev::EventAction button_action_1;
    ev::MouseButton button_state_1;
    ev::ModifierKey modifier_key_state_1;
    window_1.value()->on_mouse_button_event(
        [&](const ev::EventAction action,
            const ev::MouseButton button,
            const ev::ModifierKey modifier_key)
        {
            button_action_1 = action;
            button_state_1 = button;
            modifier_key_state_1 = modifier_key;
        }
    );
    glm::vec2 mouse_pos_1;
    window_1.value()->on_mouse_move_event([&](const glm::vec2 mouse_pos)
                                          { mouse_pos_1 = mouse_pos; });

    emulate_mouse_button_event(
        glfw_window_0, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0
    );
    if (button_action_0 != ev::EventAction::press ||
        button_state_0 != ev::MouseButton::left ||
        modifier_key_state_0 != ev::ModifierKey::none)
        return EXIT_FAILURE;

    emulate_mouse_button_event(
        glfw_window_1, GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS, 0
    );
    if (button_action_1 != ev::EventAction::press ||
        button_state_1 != ev::MouseButton::right ||
        modifier_key_state_1 != ev::ModifierKey::none)
        return EXIT_FAILURE;

    emulate_mouse_button_event(
        glfw_window_0, GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0
    );
    if (button_action_0 != ev::EventAction::release ||
        button_state_0 != ev::MouseButton::left ||
        modifier_key_state_0 != ev::ModifierKey::none)
        return EXIT_FAILURE;

    emulate_mouse_button_event(
        glfw_window_1, GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE, 0
    );
    if (button_action_1 != ev::EventAction::release ||
        button_state_1 != ev::MouseButton::right ||
        modifier_key_state_1 != ev::ModifierKey::none)
        return EXIT_FAILURE;

    emulate_mouse_button_event(
        glfw_window_0, GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS, GLFW_MOD_SHIFT
    );
    if (button_action_0 != ev::EventAction::press ||
        button_state_0 != ev::MouseButton::middle ||
        modifier_key_state_0 != ev::ModifierKey::shift)
        return EXIT_FAILURE;

    emulate_mouse_button_event(
        glfw_window_1, GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS, GLFW_MOD_CONTROL
    );
    if (button_action_1 != ev::EventAction::press ||
        button_state_1 != ev::MouseButton::middle ||
        modifier_key_state_1 != ev::ModifierKey::control)
        return EXIT_FAILURE;

    emulate_mouse_move_event(glfw_window_0, 5.0, 7.0);
    if (mouse_pos_0 != glm::vec2(5.0f, 7.0f))
        return EXIT_FAILURE;

    emulate_mouse_move_event(glfw_window_1, 6.0, 8.0);
    if (mouse_pos_1 != glm::vec2(6.0f, 8.0f))
        return EXIT_FAILURE;

    emulate_mouse_move_event(glfw_window_0, 9.0, 11.0);
    if (mouse_pos_0 != glm::vec2(9.0f, 11.0f))
        return EXIT_FAILURE;

    emulate_mouse_move_event(glfw_window_1, 10.0, 12.0);
    if (mouse_pos_1 != glm::vec2(10.0f, 12.0f))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

GLFWmousebuttonfun glfw_get_mouse_button_callback(GLFWwindow *window)
{
    GLFWmousebuttonfun callback = glfwSetMouseButtonCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, callback);
    return callback;
}

void emulate_mouse_button_event(
    GLFWwindow *glfw_window, int button, int action, int mods
)
{
    glfw_get_mouse_button_callback(glfw_window
    )(glfw_window, button, action, mods);
}

GLFWcursorposfun glfw_get_mouse_move_callback(GLFWwindow *window)
{
    GLFWcursorposfun callback = glfwSetCursorPosCallback(window, nullptr);
    glfwSetCursorPosCallback(window, callback);
    return callback;
}

void emulate_mouse_move_event(GLFWwindow *glfw_window, double xpos, double ypos)
{
    glfw_get_mouse_move_callback(glfw_window)(glfw_window, xpos, ypos);
}
