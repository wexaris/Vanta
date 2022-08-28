#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta{
    namespace Editor {

        class ViewportCameraController {
        public:
            SceneCamera Camera;

            ViewportCameraController();
            
            void OnUpdate(double delta);

            void SetTransform(const glm::mat4& transform);
            void SetTransform(const glm::vec3& position, const glm::vec3& rotation);

            glm::vec3 GetUpVector() const      { return glm::vec3{ 0.f, 1.f, 0.f }; }
            glm::vec3 GetForwardVector() const { return glm::quat(Camera.GetRotationRad()) * glm::vec3{0.f, 0.f, -1.f}; }
            glm::vec3 GetRightVector() const   { return glm::normalize(glm::cross(GetForwardVector(), GetUpVector())); }

            void OnViewportResize(uint width, uint height);

            void OnEvent(Event& e);

            bool IsActive() const { return m_IsActive; }

        private:
            static constexpr float DEFAULT_FOV = 45.f;
            static constexpr float MAX_FOV = 90.f;

            static constexpr float DEFAULT_ZOOM = DEFAULT_FOV / MAX_FOV;
            static constexpr float MIN_ZOOM = 0.1f;
            static constexpr float MAX_ZOOM = 1.f;

            bool m_IsActive = false; 
            
            float m_MovementSpeed = 5.f;
            float m_RotationSpeed = 0.2f;
            float m_Zoom = DEFAULT_ZOOM;

            bool OnMouseMove(MouseMoveEvent& e);
            bool OnMouseButtonPress(MouseButtonPressEvent& e);
            bool OnMouseButtonRelease(MouseButtonReleaseEvent& e);
            bool OnMouseScroll(MouseScrollEvent& e);
        };
    }
}
