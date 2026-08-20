#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Instance.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"
#include "Vanta/Scene/Components/CSharpScriptComponent.hpp"

namespace Vanta {
    void CSharpScriptComponent::Create(entt::entity e, Scene* scene) {
        Scripts::CSharpScriptEngine& engine = Scripts::CSharpScriptEngine::Get();
        if (!engine.EntityClassExists(ClassName))
            return;

        Entity entity(e, scene);
        Instance = engine.Instantiate(ClassName, entity);
    }

    void CSharpScriptComponent::Destroy() {
        Instance.reset();
    }
}
