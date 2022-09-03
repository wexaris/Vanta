#pragma once
#include "Vanta/Render/GraphicsAPI.hpp"

namespace Vanta {

    class RenderCommand {
    public:
        static void Init() {
            s_GraphicsAPI->Init();
        }

        static void SetViewport(uint x, uint y, uint width, uint height) {
            s_GraphicsAPI->SetViewport(x, y, width, height);
        }

        static void SetClearColor(const glm::vec4& color) {
            s_GraphicsAPI->SetClearColor(color);
        }

        static void Clear() {
            s_GraphicsAPI->Clear();
        }

        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint indexCount = 0) {
            s_GraphicsAPI->DrawIndexed(vertexArray, indexCount);
        }

        static void DrawLines(const Ref<VertexArray>& vertexArray, uint vertexCount) {
            s_GraphicsAPI->DrawLines(vertexArray, vertexCount);
        }

        static void SetLineWidth(float width) {
            s_GraphicsAPI->SetLineWidth(width);
        }

    private:
        static Box<GraphicsAPI> s_GraphicsAPI;

        RenderCommand() = delete;
    };
}
