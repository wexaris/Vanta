#include "vantapch.hpp"
#include "Vanta/Render/RenderCommand.hpp"
#include "Vanta/Render/Renderer2D.hpp"
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

    struct VertexData {
        glm::vec3 Position;
        glm::vec4 Color;
        uint TexID;
        glm::vec2 TexCoord;
        float TilingFactor;

        // Editor data
        int EntityID = -1;
    };

    struct RenderData {
        static constexpr uint MaxQuads = 0x1fff;
        static constexpr uint MaxVerts = MaxQuads * 4;
        static constexpr uint MaxIndices = MaxQuads * 6;
        static constexpr uint MaxTextureSlots = 16;

        // Render objects
        Ref<VertexArray> VAO;
        Ref<VertexBuffer> VBO;
        
        // Dynamic vertex data
        VertexData* VertexDataBuffer = nullptr;
        VertexData* VertexDataBufferPtr = nullptr;
        uint QuadIndexCount = 0;

        // Resources
        Ref<Shader> Shader;
        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint TextureSlotIdx = 1; // 0 = white texture

        Renderer2D::Statistics Stats;
    };

    RenderData s_Data;

    void Renderer2D::Init() {
        VANTA_PROFILE_RENDER_FUNCTION();

        s_Data.VAO = VertexArray::Create();

        { // Create vertex buffer
            s_Data.VBO = VertexBuffer::Create(s_Data.MaxVerts * sizeof(VertexData));
            s_Data.VBO->SetLayout({
                { Shader::DataType::Float3, "aPosition" },
                { Shader::DataType::Float4, "aColor" },
                { Shader::DataType::UInt,   "aTexID" },
                { Shader::DataType::Float2, "aTexCoords" },
                { Shader::DataType::Float,  "aTilingFactor" },
                { Shader::DataType::Int,    "aEntityID" }, });
            s_Data.VAO->AddVertexBuffer(s_Data.VBO);
        }

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

            Ref<IndexBuffer> EBO = IndexBuffer::Create(indices, s_Data.MaxIndices);
            s_Data.VAO->SetIndexBuffer(EBO);

            delete[] indices;
        }

        // Load resources
        s_Data.Shader = Shader::Create("DefaultTextureShader", "Shaders/Texture.glsl");

        constexpr uint32 white = 0xffffffff;
        s_Data.TextureSlots[0] = Texture2D::Create(1, 1);
        s_Data.TextureSlots[0]->SetData(&white, sizeof(uint32));

        // Create dynamic vertex buffer
        s_Data.VertexDataBuffer = new VertexData[s_Data.MaxVerts];
    }

    void Renderer2D::Shutdown() {
        VANTA_PROFILE_RENDER_FUNCTION();
        delete[] s_Data.VertexDataBuffer;
    }

    void Renderer2D::SceneBegin(Camera* camera) {
        VANTA_PROFILE_RENDER_FUNCTION();

        // Upload uniform to GPU
        s_Data.Shader->SetMat4("uViewProjection", camera->GetViewProjection());

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
        s_Data.VertexDataBufferPtr = s_Data.VertexDataBuffer;
        s_Data.QuadIndexCount = 0;
        s_Data.TextureSlotIdx = 1;
    }

    void Renderer2D::BatchFlush() {
        if (s_Data.QuadIndexCount == 0)
            return;

        // Upload vertex data to GPU
        usize dataSize = (usize)((uintptr_t)s_Data.VertexDataBufferPtr - (uintptr_t)s_Data.VertexDataBuffer);
        s_Data.VBO->SetData(s_Data.VertexDataBuffer, dataSize);

        // Bind resources
        s_Data.Shader->Bind();
        for (uint i = 0; i < s_Data.TextureSlotIdx; i++)
            s_Data.TextureSlots[i]->Bind(i);

        RenderCommand::DrawElement(s_Data.VAO, s_Data.QuadIndexCount);
        s_Data.Stats.DrawCalls++;
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

        if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
            NextBatch();

        for (usize i = 0; i < Quad::VERTEX_COUNT; i++) {
            s_Data.VertexDataBufferPtr->Position = transform * Quad::VERTEX_POS[i];
            s_Data.VertexDataBufferPtr->Color = color;
            s_Data.VertexDataBufferPtr->TexID = 0;
            s_Data.VertexDataBufferPtr->TexCoord = Quad::TEX_COORDS[i];
            s_Data.VertexDataBufferPtr->TilingFactor = 1;
            s_Data.VertexDataBufferPtr->EntityID = entityID;
            s_Data.VertexDataBufferPtr++;
        }
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& tex, float tilingFactor, const glm::vec4& tint, int entityID) {
        VANTA_PROFILE_RENDER_FUNCTION();

        if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
            NextBatch();

        uint texID = BatchTexture(tex);

        for (usize i = 0; i < Quad::VERTEX_COUNT; i++) {
            s_Data.VertexDataBufferPtr->Position = transform * Quad::VERTEX_POS[i];
            s_Data.VertexDataBufferPtr->Color = tint;
            s_Data.VertexDataBufferPtr->TexID = texID;
            s_Data.VertexDataBufferPtr->TexCoord = Quad::TEX_COORDS[i];
            s_Data.VertexDataBufferPtr->TilingFactor = tilingFactor;
            s_Data.VertexDataBufferPtr->EntityID = entityID;
            s_Data.VertexDataBufferPtr++;
        }
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
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
            if (*s_Data.TextureSlots[i] == *tex) {
                return i;
            }
        }

        // Make sure there is a free slot, or start a new batch
        if (s_Data.TextureSlotIdx >= s_Data.MaxTextureSlots)
            NextBatch();

        // Insert texture into next slot
        s_Data.TextureSlots[s_Data.TextureSlotIdx] = tex;
        return s_Data.TextureSlotIdx++;
    }

    Renderer2D::Statistics Renderer2D::GetStats() {
        return s_Data.Stats;
    }

    void Renderer2D::ResetStats() {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }
}
