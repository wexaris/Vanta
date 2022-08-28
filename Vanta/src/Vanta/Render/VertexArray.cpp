#include "vantapch.hpp"
#include "Vanta/Render/VertexArray.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"

#include "Platform/OpenGL/VertexArray.hpp"

namespace Vanta {

    Box<VertexArray> VertexArray::Create() {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLVertexArray>();
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }
}
