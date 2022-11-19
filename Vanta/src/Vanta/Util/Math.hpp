#pragma once
#include <glm/gtx/string_cast.hpp>

namespace Vanta {
    namespace Math {

        bool Decompose(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
        bool Decompose(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation);
    }

    template<typename Out, glm::length_t L, typename T, glm::qualifier Q>
    inline Out& operator<<(Out& out, const glm::vec<L, T, Q>& vec) {
        return out << glm::to_string(vec);
    }

    template<typename Out, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    inline Out& operator<<(Out& out, const glm::mat<C, R, T, Q>& mat) {
        return out << glm::to_string(mat);
    }

    template<typename Out, typename T, glm::qualifier Q>
    inline Out& operator<<(Out& out, const glm::qua<T, Q>& qua) {
        return out << glm::to_string(qua);
    }
}
