#pragma once
#include <entt/entt.hpp>

namespace Vanta {

    class Scene;

    class ScriptInstance;

    struct NativeScriptComponent {
        std::string ClassName;

        // Script runtime instance
        Ref<ScriptInstance> Instance = nullptr;

        void Create(entt::entity e, Scene* scene);
        void Destroy();

        NativeScriptComponent() = default;
        NativeScriptComponent(const NativeScriptComponent&) = default;
    };
}
