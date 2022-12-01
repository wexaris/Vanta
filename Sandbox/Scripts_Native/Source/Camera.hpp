#pragma once
#include <Vanta/Scripts/Native/External/NativeScript.hpp>

namespace Sandbox {

    class Camera : public Vanta::EntityScript {
        REGISTER_SCRIPT(Camera);

    public:
        void OnCreate() override;

        void OnUpdate(double) override {

        }

        void OnDestroy() override {

        }
    };
}
