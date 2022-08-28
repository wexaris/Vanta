#include "vantapch.hpp"
#include "Vanta/Render/Texture.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"

#include "Platform/OpenGL/Texture.hpp"

namespace Vanta {

    Ref<Texture2D> Texture2D::Create(uint32 width, uint32 height) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLTexture2D>(width, height);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }

    Ref<Texture2D> Texture2D::Create(const Path& path) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLTexture2D>(path);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }
}
