#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"
#include "Vanta/Scene/Components/NativeScriptComponent.hpp"

namespace Vanta {
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
