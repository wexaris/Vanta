#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Instance.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"
#include "Vanta/Scene/Components/NativeScriptComponent.hpp"

namespace Vanta {
    void NativeScriptComponent::Create(entt::entity e, Scene* scene) {
        Scripts::NativeScriptEngine& engine = Scripts::NativeScriptEngine::Get();
        if (!engine.EntityClassExists(ClassName))
            return;

        Entity entity(e, scene);
        Instance = engine.Instantiate(ClassName, entity);
    }

    void NativeScriptComponent::Destroy() {
        Instance.reset();
    }
}
