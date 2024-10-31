#pragma once

namespace Vanta {
    namespace NativeImpl {

        const glm::vec3& TransformComponent_GetPosition(UUID entityID);
        void TransformComponent_SetPosition(UUID entityID, const glm::vec3& pos);
    }
}
