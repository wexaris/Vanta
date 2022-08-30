#pragma once

namespace Vanta {
    namespace Math {

        bool Decompose(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
        bool Decompose(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation);
    }
}

inline std::ostream& operator<<(std::ostream& out, const glm::vec2 vec) {
    return out << FMT("({}, {})", vec.x, vec.y);
}

inline std::ostream& operator<<(std::ostream& out, const glm::vec3 vec) {
    return out << FMT("({}, {}, {})", vec.x, vec.y, vec.z);
}

inline std::ostream& operator<<(std::ostream& out, const glm::vec4 vec) {
    return out << FMT("({}, {}, {}, {})", vec.x, vec.y, vec.z, vec.w);
}
