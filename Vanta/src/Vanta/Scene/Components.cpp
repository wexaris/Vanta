#include "vantapch.hpp"
#include "Vanta/Scene/Components.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/SceneCamera.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {

    CameraComponent::CameraComponent() {
        Camera = NewRef<SceneCamera>(SceneCamera::Perspective());
    }

    CameraComponent::CameraComponent(Ref<SceneCamera> camera, bool fixedAspectRatio)
        : Camera(camera), FixedAspectRatio(fixedAspectRatio) {}

    void ScriptComponent::Create(entt::entity e, Scene* scene) {
        if (!CSharp::ScriptEngine::ClassExists(ClassName))
            return;

        Entity entity(e, scene);
        Instance = CSharp::ScriptEngine::Instantiate(ClassName, entity);
    }

    void ScriptComponent::Destroy() {
        Instance.reset();
    }

    void NativeScriptComponent::Create(entt::entity e, Scene* scene) {
        if (!Native::ScriptEngine::ClassExists(ClassName))
            return;

        Entity entity(e, scene);
        Instance = Native::ScriptEngine::Instantiate(ClassName, entity);
    }

    void NativeScriptComponent::Destroy() {
        Instance.reset();
    }
}
