using System;

using Vanta;

namespace Sandbox {

    public class Player : Entity {
        private TransformComponent m_Transform;
        private Rigidbody2DComponent m_Rigidbody;

        void OnCreate() {
            Log.Info("Player.OnCreate");

            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
        }

        void OnUpdate(float delta) {

            const float speed = 0.01f;
            Vector2 velocity = Vector2.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y += speed;
            if (Input.IsKeyDown(KeyCode.S))
                velocity.Y -= speed;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.X -= speed;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X += speed;

            m_Rigidbody.ApplyLinearImpulse(velocity);

            //Vector3 position = Position;
            //position += velocity * delta;
            //Position = position;
        }
    }
}