#include <Vanta/Vanta.hpp>

namespace Testing {

    class NoComponentUsageScript : public Vanta::Entity {
        VANTA_SCRIPT(NoComponentUsageScript);
        VANTA_FIELD(float, Speed);

    public:
        float Speed = 1.0f;

        void OnCreate() override {
        }

        void OnUpdate(double) override {
        }
    };

}