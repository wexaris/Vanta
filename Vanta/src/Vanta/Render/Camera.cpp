#include <vantapch.hpp>
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Render/Camera.hpp"

namespace Vanta {
    /// ///////////////////////////////////////////////////////////////////////
    /// Camera

    void Camera::SetTransform(const glm::vec3& position, const glm::quat& rotation) {
        VANTA_PROFILE_FUNCTION();
        glm::mat4 translate = glm::translate(glm::mat4(1.f), position);
        glm::mat4 rotate = glm::mat4_cast(rotation);
        //glm::mat4 scale = glm::scale(rotate, scale);
        SetTransform(translate * rotate);
    }

    void Camera::SetTransform(const glm::mat4& transform) {
        VANTA_PROFILE_FUNCTION();
        SetView(glm::inverse(transform));
    }

    void Camera::SetView(const glm::mat4& view) {
        VANTA_PROFILE_FUNCTION();
        m_ViewMatrix = view;
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    /// ///////////////////////////////////////////////////////////////////////
    /// Orthographic Camera

    OrthographicCamera::OrthographicCamera() :
        m_AspectRatio((float)Engine::Get().GetWindow().GetWidth() / (float)Engine::Get().GetWindow().GetHeight()),
        m_Bounds(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, m_Zoom, -m_Zoom)
    {
        Recalculate();
    }

    void OrthographicCamera::Resize(uint width, uint height) {
        m_AspectRatio = (float)width / height;
        Recalculate();
    }

    void OrthographicCamera::SetZoom(float zoom) {
        m_Zoom = zoom;
        Recalculate();
    }

    void OrthographicCamera::Recalculate() {
        VANTA_PROFILE_FUNCTION();
        m_Bounds = OrthographicCameraBounds(-m_AspectRatio * m_Zoom, m_AspectRatio* m_Zoom, m_Zoom, -m_Zoom);
        m_ProjectionMatrix = glm::ortho(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top, 0.1f, 100.f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    /// ///////////////////////////////////////////////////////////////////////
    /// Perspective Camera

    PerspectiveCamera::PerspectiveCamera() :
        m_AspectRatio((float)Engine::Get().GetWindow().GetWidth() / (float)Engine::Get().GetWindow().GetHeight())
    {
        Recalculate();
    }

    void PerspectiveCamera::Resize(uint width, uint height) {
        m_AspectRatio = (float)width / height;
        Recalculate();
    }

    void PerspectiveCamera::SetFOV(float fov) {
        m_FOV = fov;
        Recalculate();
    }

    void PerspectiveCamera::Recalculate() {
        VANTA_PROFILE_FUNCTION();
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, 0.1f, 100.f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}
