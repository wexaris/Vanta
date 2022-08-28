#include <vantapch.hpp>
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Render/Camera.hpp"

namespace Vanta {

    void Camera::SetView(const glm::mat4& view) {
        m_ViewMatrix = view;
        m_DirtyViewProjection = true;
    }

    void Camera::SetProjection(const glm::mat4& proj) {
        m_ProjectionMatrix = proj;
        m_DirtyViewProjection = true;
    }

    void Camera::RecalculateViewProjection() {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_DirtyViewProjection = false;
    }

#if 0
    void Camera::SetTransform(const glm::mat4& transform) {
        VANTA_PROFILE_FUNCTION();
        m_Transform = transform;
        Math::Decompose(m_Transform, m_Position, m_Rotation);
        SetView(glm::inverse(m_Transform));
    }

    void Camera::SetTransform(const glm::vec3& position, const glm::vec3& rotation) {
        VANTA_PROFILE_FUNCTION();
        m_Position = position;
        m_Rotation = glm::radians(rotation);
        m_DirtyTransform = true;
    }

    void Camera::SetPosition(const glm::vec3& position) {
        m_Position = position;
        m_DirtyTransform = true;
    }

    void Camera::SetRotation(const glm::vec3& rotation) {
        m_Rotation = glm::radians(rotation);
        m_DirtyTransform = true;
    }

    void Camera::Move(const glm::vec3& offset) {
        m_Position += offset;
        m_DirtyTransform = true;
    }

    void Camera::Rotate(const glm::vec3& offset) {
        m_Rotation += glm::radians(offset);
        m_DirtyTransform = true;
    }

    void Camera::RecalculateTransform() {
        glm::mat4 translate = glm::translate(glm::mat4(1.f), m_Position);
        glm::mat4 rotate = glm::mat4_cast(glm::quat(m_Rotation));
        m_Transform = translate * rotate;
        SetView(glm::inverse(m_Transform));
        m_DirtyTransform = false;
    }
#endif
}
