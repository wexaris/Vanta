#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"
#include "Vanta/Scene/Components/ScriptComponent.hpp"

namespace Vanta {
    void ScriptComponent::Create(entt::entity e, Scene* scene) {
        Scripts::CSharpScriptEngine& engine = Scripts::CSharpScriptEngine::Get();
        if (!engine.EntityClassExists(ClassName))
            return;

        Entity entity(e, scene);
        Instance = engine.Instantiate(ClassName, entity);
    }

    void ScriptComponent::Destroy() {
        Instance.reset();
    }
}
