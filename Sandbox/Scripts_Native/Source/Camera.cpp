#include "Camera.hpp"

using namespace Vanta;

namespace Sandbox {

    void Camera::OnCreate() {
        Log::Info("Camera::OnCreate");
    }

    void Camera::OnUpdate(double) {
        auto tr = GetComponent<TransformComponent>();

        if (tr) {
            const auto& pos = tr.GetPosition();
            Log::Debug("Here");
        }

        if (Input::IsMouseDown(Mouse::ButtonLeft))
            Log::Info("Camera::LEFT");
    }
}
