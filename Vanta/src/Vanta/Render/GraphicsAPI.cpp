#include "vantapch.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"

#include "Platform/OpenGL/Render/GraphicsAPI.hpp"

namespace Vanta {
    
    GraphicsAPI::API GraphicsAPI::s_API = GraphicsAPI::API::OpenGL;

    Box<GraphicsAPI> GraphicsAPI::Create() {
        switch (s_API) {
        case API::OpenGL: return NewBox<OpenGLGraphicsAPI>();
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }
}
