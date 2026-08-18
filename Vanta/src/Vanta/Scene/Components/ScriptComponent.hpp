#pragma once
#include <entt/entt.hpp>

namespace Vanta {

    class Scene;

    namespace Scripts {
        class ScriptInstance;
    }

    struct ScriptComponent {
        std::string ClassName;

        // Script runtime instance
        Ref<Scripts::ScriptInstance> Instance = nullptr;

        void Create(entt::entity e, Scene* scene);
        void Destroy();

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
    };
}
