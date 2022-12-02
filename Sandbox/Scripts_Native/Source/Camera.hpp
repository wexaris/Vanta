#pragma once
#include <Vanta/Scripts/Native/Module/Vanta.hpp>

namespace Sandbox {

    class Camera : public Vanta::Entity {
        REGISTER_SCRIPT(Camera);

    public:
        void OnCreate() override;

        void OnUpdate(double) override {

        }

        void OnDestroy() override {

        }
    };
}
