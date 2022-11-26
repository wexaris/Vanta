using System;

using Vanta;

namespace Sandbox {

    public class Player : Entity {
        private TransformComponent m_Transform;
        private Rigidbody2DComponent m_Rigidbody;

        public float Speed = 5.0f;

        void OnCreate() {
            Log.Info("Player.OnCreate");

            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
        }

        void OnUpdate(float delta) {
            Vector2 velocity = Vector2.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y += Speed;
            if (Input.IsKeyDown(KeyCode.S))
                velocity.Y -= Speed;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.X -= Speed;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X += Speed;

            m_Rigidbody.ApplyLinearImpulse(velocity / 50.0f);

            //Vector3 position = Position;
            //position += velocity * delta;
            //Position = position;
        }
    }
}