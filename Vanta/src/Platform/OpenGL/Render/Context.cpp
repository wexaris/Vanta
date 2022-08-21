#include "vantapch.hpp"
#include "Platform/OpenGL/Render/Context.hpp"

#include <glad/glad.h>

namespace Vanta {
    OpenGLContext::OpenGLContext(GLFWwindow* window) :
        m_Window(window)
    {
        VANTA_PROFILE_FUNCTION();

        VANTA_CORE_ASSERT(m_Window, "Cannot create a graphics context before window creation!");

        {
            VANTA_PROFILE_SCOPE("glfwMakeContextCurrent()");
            glfwMakeContextCurrent(m_Window);
        }

        {
            VANTA_PROFILE_SCOPE("gladLoadGLLoader()");
            auto good = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            VANTA_CORE_ASSERT(good, "Failed to initialize Glad!");
        }

        VANTA_CORE_INFO("OpenGL Information");
        VANTA_CORE_INFO("  Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        VANTA_CORE_INFO("  Device: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        VANTA_CORE_INFO("  Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        GLint verMajor, verMinor;
        glGetIntegerv(GL_MAJOR_VERSION, &verMajor);
        glGetIntegerv(GL_MINOR_VERSION, &verMinor);
        VANTA_CORE_ASSERT(verMajor > 4 || (verMajor == 4 && verMinor >= 3), "Vanta requires OpenGL 4.3 or newer!");
    }

    void OpenGLContext::SwapBuffers() {
        VANTA_PROFILE_RENDER_FUNCTION();
        glfwSwapBuffers(m_Window);
    }
}
