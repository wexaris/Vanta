#include "vantapch.hpp"
#include "Vanta/Render/RenderCommand.hpp"

namespace Vanta {

    Box<GraphicsAPI> RenderCommand::s_GraphicsAPI = GraphicsAPI::Create();

    void RenderCommand::Init() {
        s_GraphicsAPI->Init();
    }

    void RenderCommand::SetViewport(uint x, uint y, uint width, uint height) {
        s_GraphicsAPI->SetViewport(x, y, width, height);
    }

    void RenderCommand::Clear(const glm::vec4& color) {
        s_GraphicsAPI->Clear(color);
    }

    void RenderCommand::DrawElement(const Ref<VertexArray>& vertexArray, uint indexCount) {
        s_GraphicsAPI->DrawElement(vertexArray, indexCount);
    }
}
