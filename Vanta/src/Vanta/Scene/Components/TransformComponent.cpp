#include "vantapch.hpp"
#include "Vanta/Scene/Components/TransformComponent.hpp"

namespace Vanta {
    TransformComponent::TransformComponent(const glm::mat4& transform) {
        SetTransform(transform);
    }

    TransformComponent::TransformComponent(
        const glm::vec3& position,
        const glm::vec3& degrees,
        const glm::vec3& scale)
    {
        SetTransformDeg(position, degrees, scale);
    }

    void TransformComponent::SetTransform(const glm::mat4& transform) {
        Transform = transform;
        Math::Decompose(Transform, Position, Rotation, Scale);
    }

    void TransformComponent::SetTransformRad(
        const glm::vec3& position,
        const glm::vec3& radians,
        const glm::vec3& scale)
    {
        Position = position;
        Rotation = radians;
        Scale = scale;
        DirtyTransform = true;
    }

    void TransformComponent::SetTransformDeg(
        const glm::vec3& position,
        const glm::vec3& degrees,
        const glm::vec3& scale)
    {
        SetTransformRad(position, glm::radians(degrees), scale);
    }

    void TransformComponent::SetPosition(const glm::vec3& position) {
        Position = position;
        DirtyTransform = true;
    }

    void TransformComponent::SetRotationDeg(const glm::vec3& degrees) {
        SetRotationRad(glm::radians(degrees));
    }

    void TransformComponent::SetRotationRad(const glm::vec3& radians) {
        Rotation = radians;
        DirtyTransform = true;
    }

    void TransformComponent::SetScale(const glm::vec3& scale) {
        Scale = scale;
        DirtyTransform = true;
    }

    void TransformComponent::Recalculate() {
        auto position = glm::translate(glm::mat4(1.f), Position);
        auto rotation = glm::mat4_cast(glm::quat(Rotation));
        auto scale = glm::scale(glm::mat4(1.f), Scale);
        Transform = (glm::mat4)(position * rotation * scale);
        DirtyTransform = false;
    }
}
