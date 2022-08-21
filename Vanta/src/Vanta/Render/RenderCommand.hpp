#pragma once
#include "Vanta/Render/GraphicsAPI.hpp"

namespace Vanta {

    class RenderCommand {
    public:
        static void Init();

        static void SetViewport(uint x, uint y, uint width, uint height);

        static void Clear(const glm::vec4& color = glm::vec4(0, 0, 0, 1));

        static void DrawElement(const Ref<VertexArray>& vertexArray, uint indexCount = 0);

    private:
        static Box<GraphicsAPI> s_GraphicsAPI;

        RenderCommand() = delete;
    };
}
