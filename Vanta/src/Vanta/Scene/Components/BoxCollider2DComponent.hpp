#pragma once

struct b2ShapeId;

namespace Vanta {

    struct BoxCollider2DComponent {
        glm::vec2 Size = { 0.5f, 0.5f };
        glm::vec2 Offset = { 0.0f, 0.0f };

        float Density = 1.0f;
        float Friction = 0.4f;
        float Restitution = 0.3f; // bounciness
        float RollingResistance = 0.1f;

        // Physics runtime instance
        b2ShapeId RuntimeShape;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };
}
