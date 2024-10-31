#pragma once

namespace Vanta {

    struct CircleCollider2DComponent {
        float Radius = 0.5f;
        glm::vec2 Offset = { 0.0f, 0.0f };

        float Density = 1.0f;
        float Friction = 0.4f;
        float Restitution = 0.3f;           // bounciness
        float RestitutionThreshold = 0.5f;

        // Physics runtime instance
        void* RuntimeFixture = nullptr;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
    };
}
