#pragma once

namespace Vanta {

    struct BoxCollider2DComponent {
        glm::vec2 Size = { 0.5f, 0.5f };
        glm::vec2 Offset = { 0.0f, 0.0f };

        float Density = 1.0f;
        float Friction = 0.4f;
        float Restitution = 0.3f;           // bounciness
        float RestitutionThreshold = 0.5f;

        // Physics runtime instance
        void* RuntimeFixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };
}
