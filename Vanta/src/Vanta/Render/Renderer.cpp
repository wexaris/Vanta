#include "vantapch.hpp"
#include "Vanta/Render/Renderer.hpp"
#include "Vanta/Render/Renderer2D.hpp"

namespace Vanta {

    void Renderer::Init() {
        VANTA_PROFILE_RENDER_FUNCTION();
        RenderCommand::Init();
        Renderer2D::Init();
    }

    void Renderer::Shutdown() {
        VANTA_PROFILE_RENDER_FUNCTION();
        Renderer2D::Shutdown();
    }

    ShaderLibrary& Renderer::GetShaderLibrary() {
        static ShaderLibrary instance;
        return instance;
    }

    void Renderer::OnWindowResize(uint width, uint height) {
        RenderCommand::SetViewport(0, 0, width, height);
    }
}
