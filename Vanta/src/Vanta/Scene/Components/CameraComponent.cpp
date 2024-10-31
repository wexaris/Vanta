#include "vantapch.hpp"
#include "Vanta/Scene/SceneCamera.hpp"
#include "Vanta/Scene/Components/CameraComponent.hpp"

namespace Vanta {
    CameraComponent::CameraComponent() {
        Camera = NewRef<SceneCamera>(SceneCamera::Perspective());
    }

    CameraComponent::CameraComponent(Ref<SceneCamera> camera, bool fixedAspectRatio)
        : Camera(camera), FixedAspectRatio(fixedAspectRatio) {}
}
