#include "Camera.hpp"

using namespace Vanta;

namespace Sandbox {

    void Camera::OnCreate() {
        m_Player = GetEntityByName("Player");
        if (!m_Player) {
            Vanta::Log::Error("Failed to find player entity!");
        }
    }

    void Camera::OnUpdate(double) {
        if (!m_Player)
            return;

        auto tr = GetComponent<TransformComponent>();
        auto& playerPos = m_Player.GetComponent<TransformComponent>().GetPosition();

        if (m_Player)
            tr.SetPosition({ playerPos.X, playerPos.Y, DistanceZ });
    }
}
