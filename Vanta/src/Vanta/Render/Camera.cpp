#include "vantapch.hpp"
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
}
