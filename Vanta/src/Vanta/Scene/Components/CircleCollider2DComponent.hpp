#pragma once

struct b2ShapeId;

namespace Vanta {

    struct CircleCollider2DComponent {
        float Radius = 0.5f;
        glm::vec2 Offset = { 0.0f, 0.0f };

        float Density = 1.0f;
        float Friction = 0.4f;
        float Restitution = 0.3f; // bounciness
        float RollingResistance = 0.1f;

        // Physics runtime instance
        b2ShapeId RuntimeShape;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
    };
}
