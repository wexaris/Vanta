#include "vantapch.hpp"
#include "Vanta/Scene/Components.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Script/NativeScript.hpp"
#include "Vanta/Script/ScriptEngine.hpp"

namespace Vanta {

    void ScriptComponent::Create(entt::entity e, Scene* scene) {
        Entity entity(e, scene);
        Instance = ScriptEngine::Instantiate(ClassName, entity);
    }

    void ScriptComponent::Destroy() {
        Instance.reset();
    }

    void NativeScriptComponent::Create(entt::entity e, Scene* scene) {
        Entity entity(e, scene);
        Instance = CreateInstance();
        Instance->m_Entity = entity;
    }

    void NativeScriptComponent::Destroy() {
        DestroyInstance(Instance);
        Instance = nullptr;
    }
}
