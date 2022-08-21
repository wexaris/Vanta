#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Input/Input.hpp"

#include <GLFW/glfw3.h>

namespace Vanta {
    void Input::PollInputs() {
        glfwPollEvents();
    }

    bool Input::IsKeyPressed(KeyCode key) {
        auto window = static_cast<GLFWwindow*>(Engine::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, static_cast<int>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
    
    bool Input::IsMouseButtonPressed(MouseCode button) {
        auto window = static_cast<GLFWwindow*>(Engine::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, static_cast<int>(button));
        return state == GLFW_PRESS;
    }

    std::pair<float, float> Input::GetMousePos() {
        auto window = static_cast<GLFWwindow*>(Engine::Get().GetWindow().GetNativeWindow());
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        return { (float)xPos, (float)yPos };
    }

    float Input::GetMouseX() {
        auto [x, y] = GetMousePos();
        return x;
    }

    float Input::GetMouseY() {
        auto [x, y] = GetMousePos();
        return y;
    }
}
