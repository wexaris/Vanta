#pragma once

namespace Vanta {

    class SceneCamera;

    struct CameraComponent {
        Ref<SceneCamera> Camera = nullptr;
        bool FixedAspectRatio = false;

        CameraComponent();
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(Ref<SceneCamera> camera, bool fixedAspectRatio = false);
    };
}
