#pragma once
#include <entt/entt.hpp>

namespace Vanta {

    class Scene;

    namespace Native {
        class ScriptInstance;
    }

    struct NativeScriptComponent {
        std::string ClassName;

        // Script runtime instance
        Ref<Native::ScriptInstance> Instance = nullptr;

        void Create(entt::entity e, Scene* scene);
        void Destroy();

        NativeScriptComponent() = default;
        NativeScriptComponent(const NativeScriptComponent&) = default;
    };
}
