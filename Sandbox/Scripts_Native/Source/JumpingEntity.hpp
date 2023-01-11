#pragma once
#include <Vanta/Scripts/Native/Module/Vanta.hpp>

namespace Sandbox {

    class JumpingEntity : public Vanta::Entity {
        VANTA_SCRIPT(JumpingEntity);

    public:
        void OnCreate() override;
        void OnUpdate(double) override;
    };
}
