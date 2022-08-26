#include "vantapch.hpp"
#include "Vanta/Scene/SceneCamera.hpp"

namespace Vanta {

    SceneCamera::SceneCamera() {
        Recalculate();
    }

    void SceneCamera::SetPerspective(float fov, float nearClip, float farClip) {
        m_Projection = Projection::Perspective;
        m_PerspectiveFOV = fov;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip) {
        m_Projection = Projection::Orthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;
    }

    void SceneCamera::Resize(uint width, uint height) {
        VANTA_CORE_ASSERT(width > 0 && height > 0, "Invalid camera viewport size!");
        m_AspectRatio = (float)width / (float)height;
        Recalculate();
    }

    void SceneCamera::Recalculate() {
        switch (m_Projection) {
        case Projection::Orthographic: {
            float orthoTop    =  m_OrthographicSize * 0.5f;
            float orthoBottom = -m_OrthographicSize * 0.5f;
            float orthoLeft   = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoRight  =  m_OrthographicSize * m_AspectRatio * 0.5f;

            m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
            break;
        }
        case Projection::Perspective: {
            m_ProjectionMatrix = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
            break;
        }
        default:
            VANTA_UNREACHABLE("Invalid camera projection!");
            break;
        }
    }
}
