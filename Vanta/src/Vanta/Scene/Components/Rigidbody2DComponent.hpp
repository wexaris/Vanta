#pragma once

namespace Vanta {

    struct Rigidbody2DComponent {
        enum class BodyType { Static = 0, Dynamic, Kinematic };

        BodyType Type = BodyType::Static;
        bool FixedRotation = false;

        // Physics runtime instance
        void* RuntimeBody = nullptr;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    };
}
