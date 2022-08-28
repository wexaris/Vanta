#include "Editor/CameraController.hpp"

namespace Vanta {
    namespace Editor {

        ViewportCameraController::ViewportCameraController() :
            m_Camera(SceneCamera::Perspective(DEFAULT_FOV, 0.1f, 1000.f)) {}

        void ViewportCameraController::OnUpdate(double delta) {
            VANTA_PROFILE_FUNCTION();

            if (m_IsActive) {
                if (Input::IsKeyPressed(Key::W)) {
                    Move(GetForwardVector() * m_MovementSpeed * (float)delta);
                }
                if (Input::IsKeyPressed(Key::S)) {
                    Move(-GetForwardVector() * m_MovementSpeed * (float)delta);
                }
                if (Input::IsKeyPressed(Key::A)) {
                    Move(-GetRightVector() * m_MovementSpeed * (float)delta);
                }
                if (Input::IsKeyPressed(Key::D)) {
                    Move(GetRightVector() * m_MovementSpeed * (float)delta);
                }
            }
        }

        void ViewportCameraController::SetTransform(const glm::mat4& transform) {
            VANTA_PROFILE_FUNCTION();
            m_Transform = transform;
            Math::Decompose(m_Transform, m_Position, m_Rotation);
            m_DirtyTransform = false;
            m_Camera.SetView(glm::inverse(m_Transform));
        }

        void ViewportCameraController::SetTransform(const glm::vec3& position, const glm::vec3& rotation) {
            VANTA_PROFILE_FUNCTION();
            m_Position = position;
            m_Rotation = glm::radians(rotation);
            m_DirtyTransform = true;
        }

        void ViewportCameraController::SetPosition(const glm::vec3& position) {
            m_Position = position;
            m_DirtyTransform = true;
        }

        void ViewportCameraController::SetRotationDeg(const glm::vec3& rotation) {
            SetRotationRad(glm::radians(rotation));
        }

        void ViewportCameraController::SetRotationRad(const glm::vec3& rotation) {
            m_Rotation = rotation;
            m_DirtyTransform = true;
        }

        void ViewportCameraController::Move(const glm::vec3& offset) {
            m_Position += offset;
            m_DirtyTransform = true;
        }

        void ViewportCameraController::RotateDeg(const glm::vec3& offset) {
            RotateRad(glm::radians(offset));
        }

        void ViewportCameraController::RotateRad(const glm::vec3& offset) {
            m_Rotation += offset;
            m_DirtyTransform = true;
        }

        void ViewportCameraController::RecalculateTransform() {
            glm::mat4 translate = glm::translate(glm::mat4(1.f), m_Position);
            glm::mat4 rotate = glm::mat4_cast(glm::quat(m_Rotation));
            m_Transform = translate * rotate;
            m_DirtyTransform = false;
            m_Camera.SetView(glm::inverse(m_Transform));
        }

        void ViewportCameraController::OnViewportResize(uint width, uint height) {
            m_Camera.Resize(width, height);
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
                RotateDeg({ e.OffsetY * m_RotationSpeed, -e.OffsetX * m_RotationSpeed, 0 });
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
            if (m_IsActive) {
                m_Zoom -= e.OffsetY * 0.1f;
                m_Zoom = std::clamp(m_Zoom, MIN_ZOOM, MAX_ZOOM);
                m_Camera.SetPerspectiveFOV(MAX_FOV * m_Zoom);
                m_MovementSpeed = m_Zoom * 10.f;
                m_RotationSpeed = m_Zoom * 0.2f;
            }
            return false;
        }
    }
}
