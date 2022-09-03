#pragma once
#include "Vanta/Render/GraphicsAPI.hpp"

namespace Vanta {
    class OpenGLGraphicsAPI : public GraphicsAPI {
    public:
        OpenGLGraphicsAPI() = default;
        ~OpenGLGraphicsAPI() = default;

        void Init() override;

        void SetViewport(uint x, uint y, uint width, uint height) override;

        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;

        void DrawIndexed(const Ref<VertexArray>& vertexArray, uint indexCount) override;
        void DrawLines(const Ref<VertexArray>& vertexArray, uint vertexCount) override;

        void SetLineWidth(float width) override;
    };
}
