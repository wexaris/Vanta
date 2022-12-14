#pragma once
#include "Vanta/Render/Camera.hpp"
#include "Vanta/Render/Texture.hpp"

namespace Vanta {

    struct SpriteComponent;

    class Renderer2D {
    public:
        struct Statistics {
            usize DrawCalls = 0;
            usize QuadCount = 0;
            usize GetVertexCount() const { return QuadCount * 4; }
            usize GetIndexCount() const  { return QuadCount * 6; }
        };

        static void Init();
        static void Shutdown();

        static void SceneBegin(Camera* camera);
        static void SceneEnd();

        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& tex, float tilingFactor = 1.f, const glm::vec4& tint = glm::vec4(1.f, 1.f, 1.f, 1.f));
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& tex, float tilingFactor = 1.f, const glm::vec4& tint = glm::vec4(1.f, 1.f, 1.f, 1.f));

        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, float rot_deg, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, float rot_deg, const glm::vec4& color);
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, float rot_deg, const Ref<Texture2D>& tex, float tilingFactor = 1.f, const glm::vec4& tint = glm::vec4(1.f, 1.f, 1.f, 1.f));
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, float rot_deg, const Ref<Texture2D>& tex, float tilingFactor = 1.f, const glm::vec4& tint = glm::vec4(1.f, 1.f, 1.f, 1.f));

        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& tex, float tilingFactor = 1.f, const glm::vec4& tint = glm::vec4(1.f, 1.f, 1.f, 1.f), int entityID = -1);

        static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

        static void DrawLine(const glm::vec3& beg, const glm::vec3& end, const glm::vec4& color, int entityID = -1);

        static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, int entityID = -1);

        static void DrawSprite(const glm::mat4& transform, SpriteComponent& sprite, int entityID);
        
        static float GetLineWidth();
        static void SetLineWidth(float width);

        static Statistics GetStats();
        static void ResetStats();

    private:
        Renderer2D() = delete;
        
        static void NextBatch();
        static void BatchBegin();
        static void BatchFlush();

        static uint BatchTexture(const Ref<Texture2D>& tex);
    };
}
