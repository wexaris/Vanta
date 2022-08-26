#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta{
    namespace Editor {

        class ViewportCameraController {
        public:
            SceneCamera Camera;

            ViewportCameraController() = default;
            
            void OnUpdate(double delta);

            void SetTransform(const glm::mat4& transform);
            void SetTransform(const glm::vec3& position, const glm::vec3& rotation);

            void OnViewportResize(uint width, uint height);

            void OnEvent(Event& e);

        private:
            float m_Zoom = 1.0f;
            glm::vec3 m_Position = { 0.f, 0.f, 0.f };
            glm::vec3 m_Rotation = { 0.f, 0.f, 0.f };
            glm::mat4 m_Transform = glm::mat4(1.f);

            bool OnMouseScroll(MouseScrollEvent& e);
        };
    }
}
