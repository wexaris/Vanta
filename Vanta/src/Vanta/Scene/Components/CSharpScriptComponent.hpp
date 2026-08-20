#pragma once
#include "Vanta/Scene/Components/ScriptComponent.hpp"

namespace Vanta {

    struct CSharpScriptComponent : public ScriptComponent {
        void Create(entt::entity e, Scene* scene) override;
        void Destroy() override;
    };
}
