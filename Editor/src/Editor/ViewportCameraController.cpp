#include "Editor/ViewportCameraController.hpp"

namespace Vanta {
    namespace Editor {

        ViewportCameraController::ViewportCameraController() :
            Camera(SceneCamera::Perspective(glm::radians(DEFAULT_FOV), 0.1f, 1000.f)) {}

        void ViewportCameraController::OnUpdate(double delta) {
            VANTA_PROFILE_FUNCTION();

            if (m_IsActive) {
                if (Input::IsKeyPressed(Key::W)) {
                    Camera.Move(GetForwardVector() * m_MovementSpeed * (float)delta);
                }
                if (Input::IsKeyPressed(Key::S)) {
                    Camera.Move(-GetForwardVector() * m_MovementSpeed * (float)delta);
                }
                if (Input::IsKeyPressed(Key::A)) {
                    Camera.Move(-GetRightVector() * m_MovementSpeed * (float)delta);
                }
                if (Input::IsKeyPressed(Key::D)) {
                    Camera.Move(GetRightVector() * m_MovementSpeed * (float)delta);
                }
            }
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
            VANTA_PROFILE_FUNCTION();
            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<MouseMoveEvent>(EVENT_METHOD(ViewportCameraController::OnMouseMove));
            dispatcher.Dispatch<MouseButtonPressEvent>(EVENT_METHOD(ViewportCameraController::OnMouseButtonPress));
            dispatcher.Dispatch<MouseButtonReleaseEvent>(EVENT_METHOD(ViewportCameraController::OnMouseButtonRelease));
            dispatcher.Dispatch<MouseScrollEvent>(EVENT_METHOD(ViewportCameraController::OnMouseScroll));
        }

        bool ViewportCameraController::OnMouseMove(MouseMoveEvent& e) {
            if (m_IsActive)
                Camera.Rotate({ e.OffsetY * m_RotationSpeed, -e.OffsetX * m_RotationSpeed, 0 });
            return false;
        }

        bool ViewportCameraController::OnMouseButtonPress(MouseButtonPressEvent& e) {
            if (e.Button == Mouse::ButtonRight) {
                m_IsActive = true;
            }
            return false;
        }

        bool ViewportCameraController::OnMouseButtonRelease(MouseButtonReleaseEvent& e) {
            if (e.Button == Mouse::ButtonRight) {
                m_IsActive = false;
            }
            return false;
        }

        bool ViewportCameraController::OnMouseScroll(MouseScrollEvent& e) {
            m_Zoom -= e.OffsetY * 0.1f;
            m_Zoom = std::clamp(m_Zoom, MIN_ZOOM, MAX_ZOOM);
            Camera.SetPerspectiveFOV(MAX_FOV * m_Zoom);
            m_MovementSpeed = m_Zoom * 7.5f;
            m_RotationSpeed = m_Zoom * 0.2f;
            return false;
        }
    }
}
