#pragma once
#include <entt/entt.hpp>

namespace Vanta {

    class Scene;

    namespace CSharp {
        class ScriptInstance;
    }

    struct ScriptComponent {
        std::string ClassName;

        // Script runtime instance
        Ref<CSharp::ScriptInstance> Instance = nullptr;

        void Create(entt::entity e, Scene* scene);
        void Destroy();

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
    };
}
