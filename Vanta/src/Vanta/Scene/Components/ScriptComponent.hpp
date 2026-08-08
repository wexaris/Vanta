#pragma once
#include <entt/entt.hpp>

namespace Vanta {

    class Scene;

    class ScriptInstance;

    struct ScriptComponent {
        std::string ClassName;

        // Script runtime instance
        Ref<ScriptInstance> Instance = nullptr;

        void Create(entt::entity e, Scene* scene);
        void Destroy();

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
    };
}
