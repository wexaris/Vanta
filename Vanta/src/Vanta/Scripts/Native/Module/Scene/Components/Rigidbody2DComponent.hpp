#pragma once

namespace Vanta {
    namespace NativeImpl {

        void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, const glm::vec2& impulse, bool wake);
    }
}
