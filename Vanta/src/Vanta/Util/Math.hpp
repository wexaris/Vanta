#pragma once

namespace Vanta {
    namespace Math {

        bool Decompose(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
    }
}
