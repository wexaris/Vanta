#include "vantapch.hpp"
#include "Vanta/Scene/SceneCamera.hpp"

namespace Vanta {

    SceneCamera::SceneCamera() {
        Recalculate();
    }

    SceneCamera SceneCamera::Perspective(float fov, float nearClip, float farClip) {
        SceneCamera camera;
        camera.SetPerspective(fov, nearClip, farClip);
        return camera;
    }

    SceneCamera SceneCamera::Orthographic(float size, float nearClip, float farClip) {
        SceneCamera camera;
        camera.SetOrthographic(size, nearClip, farClip);
        return camera;
    }

    void SceneCamera::SetPerspective(float fov, float nearClip, float farClip) {
        m_Projection = Projection::Perspective;
        m_PerspectiveFOV = glm::radians(fov);
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;
        Recalculate();
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip) {
        m_Projection = Projection::Orthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;
        Recalculate();
    }

    void SceneCamera::Resize(uint width, uint height) {
        VANTA_CORE_ASSERT(width > 0 && height > 0, "Invalid camera viewport size!");
        m_AspectRatio = (float)width / (float)height;
        Recalculate();
    }

    void SceneCamera::Recalculate() {
        switch (m_Projection) {
        case Projection::Perspective: {
            glm::mat4 projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
            SetProjection(projection);
            break;
        }
        case Projection::Orthographic: {
            float orthoTop    =  m_OrthographicSize * 0.5f;
            float orthoBottom = -m_OrthographicSize * 0.5f;
            float orthoLeft   = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoRight  =  m_OrthographicSize * m_AspectRatio * 0.5f;

            glm::mat4 projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
            SetProjection(projection);
            break;
        }
        default:
            VANTA_UNREACHABLE("Invalid camera projection!");
            break;
        }
    }
}
