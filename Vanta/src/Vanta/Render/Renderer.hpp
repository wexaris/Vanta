#pragma once
#include "Vanta/Render/GraphicsAPI.hpp"
#include "Vanta/Render/RenderCommand.hpp"
#include "Vanta/Render/Shader.hpp"

namespace Vanta {

    class Renderer {
    public:
        static void Init();
        static void Shutdown();

        static ShaderLibrary& GetShaderLibrary();

        static GraphicsAPI::API GetAPI() { return GraphicsAPI::GetAPI(); }

        static void OnWindowResize(uint width, uint height);

    private:
        Renderer() = delete;
    };
}
