#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"
#include "Vanta/Scene/Components/ScriptComponent.hpp"

namespace Vanta {
    void ScriptComponent::Create(entt::entity e, Scene* scene) {
        if (!CSharp::ScriptEngine::ClassExists(ClassName))
            return;

        Entity entity(e, scene);
        Instance = CSharp::ScriptEngine::Instantiate(ClassName, entity);
    }

    void ScriptComponent::Destroy() {
        Instance.reset();
    }
}
