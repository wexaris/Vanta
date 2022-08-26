#include "Editor/ViewportCameraController.hpp"

namespace Vanta {
    namespace Editor {

        void ViewportCameraController::OnUpdate(double delta) {
            VANTA_PROFILE_FUNCTION();


        }

        void ViewportCameraController::SetTransform(const glm::mat4& transform) {
            Camera.SetTransform(transform);
        }

        void ViewportCameraController::SetTransform(const glm::vec3& position, const glm::vec3& rotation) {
            Camera.SetTransform(position, rotation);
        }

        void ViewportCameraController::OnViewportResize(uint width, uint height) {
            Camera.Resize(width, height);
        }

        void ViewportCameraController::OnEvent(Event& e) {
            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<MouseScrollEvent>(EVENT_METHOD(ViewportCameraController::OnMouseScroll));
        }

        bool ViewportCameraController::OnMouseScroll(MouseScrollEvent& e) {
            VANTA_PROFILE_FUNCTION();
            m_Zoom -= e.OffsetY * 0.25f;
            m_Zoom = std::max(m_Zoom, 0.25f);
            Camera.SetPerspectiveFOV(m_Zoom * 22.5f);
            Camera.SetOrthographicSize(m_Zoom * 10.f);
            return false;
        }
    }
}
