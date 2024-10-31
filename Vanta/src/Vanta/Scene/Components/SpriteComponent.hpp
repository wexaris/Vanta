#pragma once
#include "Vanta/Render/Texture.hpp"

namespace Vanta {

    struct SpriteComponent {
        Ref<Texture2D> Texture = nullptr;
        float TilingFactor = 1.f;
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };

        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent&) = default;
        SpriteComponent(const glm::vec4& color)
            : SpriteComponent(nullptr, color) {}
        SpriteComponent(const Ref<Texture2D>& texture, const glm::vec4& tint)
            : Texture(texture), Color(tint) {}
    };
}
