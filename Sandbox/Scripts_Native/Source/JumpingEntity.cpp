#include "JumpingEntity.hpp"

using namespace Vanta;

namespace Sandbox {

    void JumpingEntity::OnCreate() {
        auto sp = GetComponent<SpriteComponent>();
        if (sp) {
            float r = (float)(rand() % 255) * 0.01f;
            float g = (float)(rand() % 255) * 0.01f;
            float b = (float)(rand() % 255) * 0.01f;
            float a = (float)(rand() % 255) * 0.01f;
            sp.SetColor({ r, g, b, a });
        }
    }

    void JumpingEntity::OnUpdate(double delta) {
        if (!delta)
            return;

        if (rand() % (int)(100 / delta) < 10) {
            auto rb = GetComponent<Rigidbody2DComponent>();
            if (rb) {
                rb.ApplyLinearImpulse({ 0.f, 2.f });
            }

            auto sp = GetComponent<SpriteComponent>();
            if (sp) {
                float r = (float)(rand() % 255) * 0.01f;
                float g = (float)(rand() % 255) * 0.01f;
                float b = (float)(rand() % 255) * 0.01f;
                float a = (float)(rand() % 255) * 0.01f;
                sp.SetColor({ r, g, b, a });
            }
        }
    }
}
