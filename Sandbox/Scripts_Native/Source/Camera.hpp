#pragma once
#include <Vanta/Scripts/Native/Module/Vanta.hpp>

namespace Sandbox {

    class Camera : public Vanta::Entity {
        VANTA_SCRIPT(Camera);

    public:
        float DistanceZ = 20.f;

        void OnCreate() override;
        void OnUpdate(double) override;

    private:
        Vanta::Entity m_Player;
    };
}
