#include <vanta-test-utils/CoreTestsCommon.hpp>

namespace Testing {

    bool TestMathDecompose() {
        glm::vec3 position_0 = { 1.0, 2.0, 3.0 };
        glm::vec3 rotation_0 = glm::radians(glm::vec3{ 4.0, 5.0, 6.0 });
        glm::vec3 scale_0 = { 7.0, 8.0, 9.0 };

        glm::mat4 transform =
            glm::translate(glm::mat4(1.f), position_0) *
            glm::mat4_cast(glm::quat(rotation_0)) *
            glm::scale(glm::mat4(1.f), scale_0);

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        Math::Decompose(transform, position, rotation, scale);

        float tolerance = 0.00001f;
        {
            auto offX = abs(position.x - position_0.x);
            auto offY = abs(position.y - position_0.y);
            auto offZ = abs(position.z - position_0.z);
            TRUE_OR_FAIL(offX < tolerance && offY < tolerance && offZ < tolerance);
        }
        {
            auto offX = abs(rotation.x - rotation_0.x);
            auto offY = abs(rotation.y - rotation_0.y);
            auto offZ = abs(rotation.z - rotation_0.z);
            TRUE_OR_FAIL(offX < tolerance && offY < tolerance && offZ < tolerance);
        }
        {
            auto offX = abs(scale.x - scale_0.x);
            auto offY = abs(scale.y - scale_0.y);
            auto offZ = abs(scale.z - scale_0.z);
            TRUE_OR_FAIL(offX < tolerance && offY < tolerance && offZ < tolerance);
        }

        return true;
    }
}
