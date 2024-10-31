#pragma once
#include <Vanta/Vanta.hpp>

namespace Sandbox {

    class Camera : public Vanta::Entity {
        VANTA_SCRIPT(Camera);
        VANTA_FIELD(float, DistanceZ);

    public:
        float DistanceZ = 20.f;

        void OnCreate() override;
        void OnUpdate(double) override;

    private:
        Vanta::Entity m_Player;
    };
}
