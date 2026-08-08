#include <Vanta/Vanta.hpp>

namespace Testing {

    class TestJumpingEntity : public Vanta::Entity {
        VANTA_SCRIPT(TestJumpingEntity);

    public:
        void OnCreate() override {
            auto sprite = GetComponent<Vanta::SpriteComponent>();
            if (sprite) {
                sprite.SetColor({ 0.2f, 0.6f, 0.9f, 1.0f });
            }
        }

        void OnUpdate(double delta) override {
            if (delta <= 0.0)
                return;

            auto rb = GetComponent<Vanta::Rigidbody2DComponent>();
            if (rb) {
                rb.ApplyLinearImpulse({ 0.0f, 1.0f }, true);
            }
        }
    };

    class TestCameraFollower : public Vanta::Entity {
        VANTA_SCRIPT(TestCameraFollower);
        VANTA_FIELD(float, DistanceZ);

    public:
        float DistanceZ = 20.0f;

        void OnCreate() override {
            m_Player = GetEntityByName("Player");
        }

        void OnUpdate(double) override {
            if (!m_Player)
                return;

            auto tr = GetComponent<Vanta::TransformComponent>();
            auto playerTr = m_Player.GetComponent<Vanta::TransformComponent>();
            if (tr && playerTr) {
                const auto& playerPos = playerTr.GetPosition();
                tr.SetPosition({ playerPos.X, playerPos.Y, DistanceZ });
            }
        }

    private:
        Vanta::Entity m_Player;
    };
}