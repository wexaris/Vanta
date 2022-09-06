#include "vantapch.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"
#include "Vanta/Render/UniformBuffer.hpp"

#include "Platform/OpenGL/UniformBuffer.hpp"

namespace Vanta {

    Ref<UniformBuffer> UniformBuffer::Create(uint32 size, uint32 binding) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLUniformBuffer>(size, binding);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }
}
