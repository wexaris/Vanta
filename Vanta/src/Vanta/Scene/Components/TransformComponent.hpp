#pragma once
#include "Vanta/Util/Math.hpp"

namespace Vanta {

    struct TransformComponent {
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& transform);
        TransformComponent(const glm::vec3& position, const glm::vec3& degrees, const glm::vec3& scale);

        void SetTransform(const glm::mat4& transform);
        void SetTransformRad(const glm::vec3& position, const glm::vec3& radians, const glm::vec3& scale);
        void SetTransformDeg(const glm::vec3& position, const glm::vec3& degrees, const glm::vec3& scale);

        void SetPosition(const glm::vec3& position);
        void SetRotationDeg(const glm::vec3& degrees);
        void SetRotationRad(const glm::vec3& radians);
        void SetScale(const glm::vec3& scale);

        const glm::mat4& GetTransform() {
            if (DirtyTransform)
                Recalculate();
            return Transform;
        }

        const glm::vec3& GetPosition() const { return Position; }

        const glm::vec3& GetRotationRadians() const { return Rotation; }
        glm::vec3 GetRotationDegrees() const { return glm::degrees(Rotation); }

        const glm::vec3& GetScale() const { return Scale; }

    private:
        glm::mat4 Transform = glm::mat4(1.f);

        glm::vec3 Position = { 0.f, 0.f, 0.f }; // Position
        glm::vec3 Rotation = { 0.f, 0.f, 0.f }; // Rotation in radians
        glm::vec3 Scale = { 1.f, 1.f, 1.f };    // Scale

        bool DirtyTransform = false;

        void Recalculate();
    };
}
