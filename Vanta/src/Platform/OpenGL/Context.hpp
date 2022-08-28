#pragma once
#include "Vanta/Render/GraphicsContext.hpp"

#include <GLFW/glfw3.h>

namespace Vanta {
    class OpenGLContext : public GraphicsContext {
    public:
        OpenGLContext(GLFWwindow* window);
        ~OpenGLContext() = default;

        void SwapBuffers() override;

    private:
        GLFWwindow* m_Window;
    };
}
