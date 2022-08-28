#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta{
    namespace Editor {

        class ViewportCameraController {
        public:
            ViewportCameraController();
            
            void OnUpdate(double delta);

            void SetTransform(const glm::mat4& transform);
            void SetTransform(const glm::vec3& position, const glm::vec3& rotation);

            void SetPosition(const glm::vec3& position);
            void SetRotationDeg(const glm::vec3& rotation);
            void SetRotationRad(const glm::vec3& rotation);

            void Move(const glm::vec3& offset);
            void RotateDeg(const glm::vec3& offset);
            void RotateRad(const glm::vec3& offset);

            const glm::vec3& GetPosition() const    { return m_Position; }
            const glm::vec3& GetRotationRad() const { return m_Rotation; }
            glm::vec3 GetRotationDeg() const        { return glm::degrees(m_Rotation); }

            glm::vec3 GetUpVector() const      { return glm::vec3{ 0.f, 1.f, 0.f }; }
            glm::vec3 GetForwardVector() const { return glm::quat(m_Rotation) * glm::vec3{0.f, 0.f, -1.f}; }
            glm::vec3 GetRightVector() const   { return glm::normalize(glm::cross(GetForwardVector(), GetUpVector())); }

            SceneCamera& GetCamera() {
                if (m_DirtyTransform) RecalculateTransform();
                return m_Camera;
            }

            void OnViewportResize(uint width, uint height);

            void OnEvent(Event& e);

            bool IsActive() const { return m_IsActive; }

        private:
            static constexpr float DEFAULT_FOV = 45.f;
            static constexpr float MAX_FOV = 90.f;

            static constexpr float DEFAULT_ZOOM = DEFAULT_FOV / MAX_FOV;
            static constexpr float MIN_ZOOM = 0.1f;
            static constexpr float MAX_ZOOM = 1.f;

            SceneCamera m_Camera;

            glm::vec3 m_Position = { 0.f, 0.f, 0.f };
            glm::vec3 m_Rotation = { 0.f, 0.f, 0.f }; // Rotation in radians
            glm::mat4 m_Transform = glm::mat4(1.f);
            bool m_DirtyTransform = false;
            
            float m_MovementSpeed = 5.f;
            float m_RotationSpeed = 0.2f;
            float m_Zoom = DEFAULT_ZOOM;

            bool m_IsActive = false; // If currently interacting with viewport

            void RecalculateTransform();

            bool OnMouseMove(MouseMoveEvent& e);
            bool OnMouseButtonPress(MouseButtonPressEvent& e);
            bool OnMouseButtonRelease(MouseButtonReleaseEvent& e);
            bool OnMouseScroll(MouseScrollEvent& e);
        };
    }
}
