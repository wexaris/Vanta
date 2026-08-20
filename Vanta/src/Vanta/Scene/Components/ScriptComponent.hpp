#pragma once
#include <entt/entt.hpp>

namespace Vanta {

    class Scene;

    namespace Scripts {
        class ScriptInstance;
    }

    struct ScriptComponent {
        std::string ClassName;
        Ref<Scripts::ScriptInstance> Instance = nullptr;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
        virtual ~ScriptComponent() = default;

        virtual void Create(entt::entity e, Scene* scene) = 0;
        virtual void Destroy() = 0;
    };
}
