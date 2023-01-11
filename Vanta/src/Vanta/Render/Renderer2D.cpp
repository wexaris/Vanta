#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Render/RenderCommand.hpp"
#include "Vanta/Render/Renderer2D.hpp"
#include "Vanta/Render/UniformBuffer.hpp"
#include "Vanta/Render/VertexArray.hpp"
#include "Vanta/Scene/Components.hpp"

namespace Vanta {

    struct Quad {
        static constexpr uint VERTEX_COUNT = 4;
        static constexpr glm::vec4 VERTEX_POS[] = {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f,  0.5f, 0.0f, 1.0f },
        };
        static constexpr glm::vec2 TEX_COORDS[] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f },
        };
    };

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoords;
        float TilingFactor;
        int TexID;

        // Editor data
        int EntityID = -1;

        static BufferLayout Layout() {
            return {
                { Shader::DataType::Float3, "aPosition" },
                { Shader::DataType::Float4, "aColor" },
                { Shader::DataType::Float2, "aTexCoords" },
                { Shader::DataType::Float,  "aTilingFactor" },
                { Shader::DataType::Int,    "aTexID" },
                { Shader::DataType::Int,    "aEntityID" },
            };
        }
    };

    struct CircleVertex {
        glm::vec3 WorldPosition;
        glm::vec3 LocalPosition;
        glm::vec4 Color;
        float Thickness;
        float Fade;

        // Editor data
        int EntityID = -1;

        static BufferLayout Layout() {
            return {
                { Shader::DataType::Float3, "aWorldPosition" },
                { Shader::DataType::Float3, "aLocalPosition" },
                { Shader::DataType::Float4, "aColor" },
                { Shader::DataType::Float,  "aThickness" },
                { Shader::DataType::Float,  "aFade" },
                { Shader::DataType::Int,    "aEntityID" },
            };
        }
    };

    struct LineVertex {
        glm::vec3 Position;
        glm::vec4 Color;

        // Editor data
        int EntityID = -1;

        static BufferLayout Layout() {
            return {
                { Shader::DataType::Float3, "aPosition" },
                { Shader::DataType::Float4, "aColor" },
                { Shader::DataType::Int,    "aEntityID" },
            };
        }
    };

    struct RenderData {
        static constexpr uint MaxQuads = 0x1fff;
        static constexpr uint MaxVerts = MaxQuads * 4;
        static constexpr uint MaxIndices = MaxQuads * 6;
        static constexpr uint MaxTextureSlots = 16;

        // Render objects
        Ref<VertexArray> QuadVAO;
        Ref<VertexBuffer> QuadVBO;
        Ref<Shader> QuadShader;

        Ref<VertexArray> CircleVAO;
        Ref<VertexBuffer> CircleVBO;
        Ref<Shader> CircleShader;

        Ref<VertexArray> LineVAO;
        Ref<VertexBuffer> LineVBO;
        Ref<Shader> LineShader;

        // Vertex buffers
        QuadVertex* QuadVertexBuffer = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;
        uint QuadIndexCount = 0;

        CircleVertex* CircleVertexBuffer = nullptr;
        CircleVertex* CircleVertexBufferPtr = nullptr;
        uint CircleIndexCount = 0;

        LineVertex* LineVertexBuffer = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;
        uint LineVertexCount = 0;

        // Textures
        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint TextureSlotIdx = 1; // 0 = white texture

        // Uniforms
        struct CameraData {
            glm::mat4 ViewProjection;
        };
        CameraData CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;

        // Misc
        float LineWidth = 2.0f;

        Renderer2D::Statistics Stats;
    };

    static RenderData s_Data;

    void Renderer2D::Init() {
        VANTA_PROFILE_RENDER_FUNCTION();

        s_Data.QuadVAO = VertexArray::Create();
        s_Data.CircleVAO = VertexArray::Create();
        s_Data.LineVAO = VertexArray::Create();

        // Create VBOs
        s_Data.QuadVBO = VertexBuffer::Create(s_Data.MaxVerts * sizeof(QuadVertex));
        s_Data.QuadVBO->SetLayout(QuadVertex::Layout());
        s_Data.QuadVAO->AddVertexBuffer(s_Data.QuadVBO);

        s_Data.CircleVBO = VertexBuffer::Create(s_Data.MaxVerts * sizeof(CircleVertex));
        s_Data.CircleVBO->SetLayout(CircleVertex::Layout());
        s_Data.CircleVAO->AddVertexBuffer(s_Data.CircleVBO);

        s_Data.LineVBO = VertexBuffer::Create(s_Data.MaxVerts * sizeof(LineVertex));
        s_Data.LineVBO->SetLayout(LineVertex::Layout());
        s_Data.LineVAO->AddVertexBuffer(s_Data.LineVBO);

        { // Create index buffer
            uint* indices = new uint[s_Data.MaxIndices];

            uint offset = 0;
            for (uint i = 0; i < s_Data.MaxIndices; i += 6) {
                indices[i + 0] = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;
                indices[i + 3] = offset + 2;
                indices[i + 4] = offset + 3;
                indices[i + 5] = offset + 0;
                offset += 4;
            }

            Ref<IndexBuffer> qib = IndexBuffer::Create(indices, s_Data.MaxIndices);
            s_Data.QuadVAO->SetIndexBuffer(qib);

            Ref<IndexBuffer> cib = IndexBuffer::Create(indices, s_Data.MaxIndices);
            s_Data.CircleVAO->SetIndexBuffer(cib);

            delete[] indices;
        }

        // Create dynamic vertex buffers
        s_Data.QuadVertexBuffer = new QuadVertex[s_Data.MaxVerts];
        s_Data.CircleVertexBuffer = new CircleVertex[s_Data.MaxVerts];
        s_Data.LineVertexBuffer = new LineVertex[s_Data.MaxVerts];

        // Load shaders
        s_Data.QuadShader = Shader::Create(Engine::RuntimeResourceDirectory() / "Shaders/Renderer2D_Quad.glsl");
        s_Data.CircleShader = Shader::Create(Engine::RuntimeResourceDirectory() / "Shaders/Renderer2D_Circle.glsl");
        s_Data.LineShader = Shader::Create(Engine::RuntimeResourceDirectory() / "Shaders/Renderer2D_Line.glsl");

        // Setup white texture
        constexpr uint32 white = 0xffffffff;
        s_Data.TextureSlots[0] = Texture2D::Create(1, 1);
        s_Data.TextureSlots[0]->SetData(&white, sizeof(uint32));

        // Setup uniforms
        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(RenderData::CameraData), 0);
    }

    void Renderer2D::Shutdown() {
        VANTA_PROFILE_RENDER_FUNCTION();
        delete[] s_Data.QuadVertexBuffer;
        delete[] s_Data.CircleVertexBuffer;
        delete[] s_Data.LineVertexBuffer;
    }

    void Renderer2D::SceneBegin(Camera* camera) {
        VANTA_PROFILE_RENDER_FUNCTION();

        // Upload uniforms to GPU
        s_Data.CameraBuffer.ViewProjection = camera->GetViewProjection();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RenderData::CameraData));

        BatchBegin();
    }

    void Renderer2D::SceneEnd() {
        VANTA_PROFILE_RENDER_FUNCTION();
        BatchFlush();
    }

    void Renderer2D::NextBatch() {
        BatchFlush();
        BatchBegin();
    }

    void Renderer2D::BatchBegin() {
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBuffer;
        s_Data.QuadIndexCount = 0;

        s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBuffer;
        s_Data.CircleIndexCount = 0;

        s_Data.LineVertexBufferPtr = s_Data.LineVertexBuffer;
        s_Data.LineVertexCount = 0;

        s_Data.TextureSlotIdx = 1;
    }

    void Renderer2D::BatchFlush() {
        // Draw quads
        if (s_Data.QuadIndexCount != 0) {
            usize dataSize = (usize)((uintptr_t)s_Data.QuadVertexBufferPtr - (uintptr_t)s_Data.QuadVertexBuffer);
            s_Data.QuadVBO->SetData(s_Data.QuadVertexBuffer, dataSize);

            // Bind resources
            s_Data.QuadShader->Bind();

            for (uint i = 0; i < s_Data.TextureSlotIdx; i++)
                s_Data.TextureSlots[i]->Bind(i);

            RenderCommand::DrawIndexed(s_Data.QuadVAO, s_Data.QuadIndexCount);
            s_Data.Stats.DrawCalls++; 
        }

        // Draw circles
        if (s_Data.CircleIndexCount != 0) {
            usize dataSize = (usize)((uintptr_t)s_Data.CircleVertexBufferPtr - (uintptr_t)s_Data.CircleVertexBuffer);
            s_Data.CircleVBO->SetData(s_Data.CircleVertexBuffer, dataSize);

            // Bind resources
            s_Data.CircleShader->Bind();

            RenderCommand::DrawIndexed(s_Data.CircleVAO, s_Data.CircleIndexCount);
            s_Data.Stats.DrawCalls++;
        }

        // Draw lines
        if (s_Data.LineVertexCount != 0) {
            usize dataSize = (usize)((uintptr_t)s_Data.LineVertexBufferPtr - (uintptr_t)s_Data.LineVertexBuffer);
            s_Data.LineVBO->SetData(s_Data.LineVertexBuffer, dataSize);

            // Bind resources
            s_Data.LineShader->Bind();

            RenderCommand::SetLineWidth(s_Data.LineWidth);
            RenderCommand::DrawLines(s_Data.LineVAO, s_Data.LineVertexCount);
            s_Data.Stats.DrawCalls++;
        }
    }

    // translation * size
    static glm::mat4 CalcTransformMatrix(const glm::vec3& pos, const glm::vec2& size) {
        glm::mat4 transform(1.f);
        transform = glm::translate(transform, pos);
        transform = glm::scale(transform, { size.x, size.y, 1.f });
        return transform;
    }

    // translation * rotation * size
    static glm::mat4 CalcTransformMatrix(const glm::vec3& pos, const glm::vec2& size, float rot_deg) {
        glm::mat4 transform(1.f);
        transform = glm::translate(transform, pos);
        transform = glm::rotate(transform, glm::radians(rot_deg), glm::vec3(0.f, 0.f, 1.f));
        transform = glm::scale(transform, { size.x, size.y, 1.f });
        return transform;
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color) {
        DrawQuad(glm::vec3(pos.x, pos.y, 0.f), size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color) {
        VANTA_PROFILE_RENDER_FUNCTION();
        glm::mat4 transform = CalcTransformMatrix(pos, size);
        DrawQuad(transform, color);
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& tex, float tilingFactor, const glm::vec4& tint) {
        DrawQuad(glm::vec3(pos.x, pos.y, 0.f), size, tex, tilingFactor, tint);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& tex, float tilingFactor, const glm::vec4& tint) {
        VANTA_PROFILE_RENDER_FUNCTION();
        glm::mat4 transform = CalcTransformMatrix(pos, size);
        DrawQuad(transform, tex, tilingFactor, tint);
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, float rot_deg, const glm::vec4& color) {
        DrawQuad(glm::vec3(pos.x, pos.y, 0.f), size, rot_deg, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, float rot_deg, const glm::vec4& color) {
        VANTA_PROFILE_RENDER_FUNCTION();
        glm::mat4 transform = CalcTransformMatrix(pos, size, rot_deg);
        DrawQuad(transform, color);
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, float rot_deg, const Ref<Texture2D>& tex, float tilingFactor, const glm::vec4& tint) {
        DrawQuad(glm::vec3(pos.x, pos.y, 0.f), size, rot_deg, tex, tilingFactor, tint);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, float rot_deg, const Ref<Texture2D>& tex, float tilingFactor, const glm::vec4& tint) {
        VANTA_PROFILE_RENDER_FUNCTION();
        glm::mat4 transform = CalcTransformMatrix(pos, size, rot_deg);
        DrawQuad(transform, tex, tilingFactor, tint);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID) {
        VANTA_PROFILE_RENDER_FUNCTION();

        if (s_Data.QuadIndexCount >= s_Data.MaxIndices - 6)
            NextBatch();

        for (usize i = 0; i < Quad::VERTEX_COUNT; i++) {
            s_Data.QuadVertexBufferPtr->Position = transform * Quad::VERTEX_POS[i];
            s_Data.QuadVertexBufferPtr->Color = color;
            s_Data.QuadVertexBufferPtr->TexID = 0;
            s_Data.QuadVertexBufferPtr->TexCoords = Quad::TEX_COORDS[i];
            s_Data.QuadVertexBufferPtr->TilingFactor = 1;
            s_Data.QuadVertexBufferPtr->EntityID = entityID;
            s_Data.QuadVertexBufferPtr++;
        }
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& tex, float tilingFactor, const glm::vec4& tint, int entityID) {
        VANTA_PROFILE_RENDER_FUNCTION();

        if (s_Data.QuadIndexCount >= s_Data.MaxIndices - 6)
            NextBatch();

        uint texID = BatchTexture(tex);

        for (usize i = 0; i < Quad::VERTEX_COUNT; i++) {
            s_Data.QuadVertexBufferPtr->Position = transform * Quad::VERTEX_POS[i];
            s_Data.QuadVertexBufferPtr->Color = tint;
            s_Data.QuadVertexBufferPtr->TexID = texID;
            s_Data.QuadVertexBufferPtr->TexCoords = Quad::TEX_COORDS[i];
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
            s_Data.QuadVertexBufferPtr->EntityID = entityID;
            s_Data.QuadVertexBufferPtr++;
        }
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID) {
        VANTA_PROFILE_RENDER_FUNCTION();

        if (s_Data.CircleIndexCount >= s_Data.MaxIndices - 6)
            NextBatch(); 

        for (usize i = 0; i < Quad::VERTEX_COUNT; i++) {
            s_Data.CircleVertexBufferPtr->WorldPosition = transform * Quad::VERTEX_POS[i];
            s_Data.CircleVertexBufferPtr->LocalPosition = Quad::VERTEX_POS[i] *2.0f;
            s_Data.CircleVertexBufferPtr->Color = color;
            s_Data.CircleVertexBufferPtr->Thickness = thickness;
            s_Data.CircleVertexBufferPtr->Fade = fade;
            s_Data.CircleVertexBufferPtr->EntityID = entityID;
            s_Data.CircleVertexBufferPtr++;
        }
        s_Data.CircleIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawLine(const glm::vec3& beg, const glm::vec3& end, const glm::vec4& color, int entityID) {
        VANTA_PROFILE_RENDER_FUNCTION();

        if (s_Data.LineVertexCount >= s_Data.MaxVerts - 2)
            NextBatch();

        s_Data.LineVertexBufferPtr->Position = beg;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexBufferPtr->Position = end;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexCount += 2;
    }

    void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID) {
        VANTA_PROFILE_RENDER_FUNCTION();

        glm::vec3 lineVertices[4];
        for (usize i = 0; i < 4; i++)
            lineVertices[i] = transform * Quad::VERTEX_POS[i];

        DrawLine(lineVertices[0], lineVertices[1], color, entityID);
        DrawLine(lineVertices[1], lineVertices[2], color, entityID);
        DrawLine(lineVertices[2], lineVertices[3], color, entityID);
        DrawLine(lineVertices[3], lineVertices[0], color, entityID);
    }

    void Renderer2D::DrawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, int entityID) {
        VANTA_PROFILE_RENDER_FUNCTION();

        glm::vec3 p0 = glm::vec3(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f, pos.z);
        glm::vec3 p1 = glm::vec3(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f, pos.z);
        glm::vec3 p2 = glm::vec3(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f, pos.z);
        glm::vec3 p3 = glm::vec3(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f, pos.z);

        DrawLine(p0, p1, color, entityID);
        DrawLine(p1, p2, color, entityID);
        DrawLine(p2, p3, color, entityID);
        DrawLine(p3, p0, color, entityID);
    }

    void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteComponent& sprite, int entityID) {
        if (sprite.Texture)
            DrawQuad(transform, sprite.Texture, sprite.TilingFactor, sprite.Color, entityID);
        else
            DrawQuad(transform, sprite.Color, entityID);
    }

    uint Renderer2D::BatchTexture(const Ref<Texture2D>& tex) {
        if (!tex)
            return 0;

        // Find texture, if it's already queued
        for (uint i = 1; i < s_Data.TextureSlotIdx; i++) {
            if (*s_Data.TextureSlots[i] == *tex)
                return i;
        }

        // Make sure there is a free slot, or start a new batch
        if (s_Data.TextureSlotIdx >= s_Data.MaxTextureSlots)
            NextBatch();

        // Insert texture into next slot
        s_Data.TextureSlots[s_Data.TextureSlotIdx] = tex;
        return s_Data.TextureSlotIdx++;
    }

    float Renderer2D::GetLineWidth() {
        return s_Data.LineWidth;
    }

    void Renderer2D::SetLineWidth(float width) {
        s_Data.LineWidth = width;
    }

    Renderer2D::Statistics Renderer2D::GetStats() {
        return s_Data.Stats;
    }

    void Renderer2D::ResetStats() {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }
}
