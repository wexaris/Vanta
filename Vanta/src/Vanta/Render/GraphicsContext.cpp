#include "vantapch.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"
#include "Vanta/Render/GraphicsContext.hpp"

#include "Platform/OpenGL/Context.hpp"

namespace Vanta {
    Box<GraphicsContext> GraphicsContext::Create(void* window) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLContext>((GLFWwindow*)window);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }
}
