#include "vantapch.hpp"
#include "Vanta/Render/Framebuffer.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"

#include "Platform/OpenGL/Framebuffer.hpp"

namespace Vanta {

    Ref<Framebuffer> Framebuffer::Create(const FramebufferParams& params) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLFramebuffer>(params);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }
}
