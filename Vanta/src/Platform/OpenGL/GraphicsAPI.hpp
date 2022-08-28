#pragma once
#include "Vanta/Render/GraphicsAPI.hpp"

namespace Vanta {
    class OpenGLGraphicsAPI : public GraphicsAPI {
    public:
        OpenGLGraphicsAPI() = default;
        ~OpenGLGraphicsAPI() = default;

        void Init() override;

        void Clear(const glm::vec4& color) override;

        void DrawElement(const Ref<VertexArray>& vertexArray, uint indexCount) override;

        void SetViewport(uint x, uint y, uint width, uint height) override;
    };
}
