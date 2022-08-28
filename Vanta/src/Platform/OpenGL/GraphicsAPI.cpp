#include "vantapch.hpp"
#include "Platform/OpenGL/GraphicsAPI.hpp"

#include <glad/glad.h>

namespace Vanta {
    void OpenGLMessage(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/) {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:         VANTA_CORE_CRITICAL(message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:       VANTA_CORE_ERROR(message); return;
        case GL_DEBUG_SEVERITY_LOW:          VANTA_CORE_WARN(message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: VANTA_CORE_INFO(message); return;
        default:
            VANTA_UNREACHABLE("Unknown OpenGL message severity level!");
        }
    }

    void OpenGLGraphicsAPI::Init() {
        VANTA_PROFILE_FUNCTION();

#ifdef VANTA_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback((GLDEBUGPROC)OpenGLMessage, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLGraphicsAPI::SetViewport(uint x, uint y, uint width, uint height) {
        VANTA_PROFILE_RENDER_FUNCTION();
        glViewport(x, y, width, height);
    }

    void OpenGLGraphicsAPI::Clear(const glm::vec4& color) {
        VANTA_PROFILE_RENDER_FUNCTION();
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLGraphicsAPI::DrawElement(const Ref<VertexArray>& vertexArray, uint indexCount) {
        VANTA_PROFILE_RENDER_FUNCTION();
        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
}
