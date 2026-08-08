#pragma once

struct b2BodyId;

namespace Vanta {

    struct Rigidbody2DComponent {
        enum class BodyType { Static = 0, Dynamic, Kinematic };

        BodyType Type = BodyType::Static;
        bool FixedRotation = false;

        // Physics runtime object handle
        b2BodyId RuntimeBody;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    };
}
