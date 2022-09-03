#pragma once 
#include "Vanta/Render/VertexArray.hpp"

namespace Vanta {
    class GraphicsAPI {
    public:
        enum API {
            None = 0,
            OpenGL = 1
        };
        
        virtual ~GraphicsAPI() = default;

        virtual void Init() = 0;

        virtual void SetViewport(uint x, uint y, uint width, uint height) = 0;

        virtual void SetClearColor(const glm::vec4& color = glm::vec4(0.2f, 0.2f, 0.2f, 1.f)) = 0;
        virtual void Clear() = 0;

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint indexCount) = 0;
        virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint vertexCount) = 0;

        virtual void SetLineWidth(float width) = 0;

        static API GetAPI()         { return s_API; }
        static void SetAPI(API api) { s_API = api; }

        static Box<GraphicsAPI> Create();

    protected:
        static API s_API;
    };
}
