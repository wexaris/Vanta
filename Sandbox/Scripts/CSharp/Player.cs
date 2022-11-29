using System;

using Vanta;

namespace Sandbox {

    public class Player : Entity {
        
        public float Speed = 5.0f;

        private Camera Camera;
        private Rigidbody2DComponent Rigidbody;

        void OnCreate() {
            Log.Info("Player.OnCreate");

            Camera = GetEntityByName("Camera")?.As<Camera>();
            Rigidbody = GetComponent<Rigidbody2DComponent>();
        }

        void OnUpdate(float delta) {
            Vector2 velocity = Vector2.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y += Speed ;
            if (Input.IsKeyDown(KeyCode.S))
                velocity.Y -= Speed;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.X -= Speed;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X += Speed;

            if (Camera) {
                if (Input.IsKeyDown(KeyCode.Q))
                    Camera.DistanceZ += Speed * delta;
                if (Input.IsKeyDown(KeyCode.E))
                    Camera.DistanceZ -= Speed * delta;
            }

            Rigidbody.ApplyLinearImpulse(velocity * delta);
        }
    }
}