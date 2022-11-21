using System;

using Vanta;

namespace Sandbox {

    public class Player : Entity {
        void OnCreate() {
            Internal.Info("Player.OnCreate");
        }

        void OnUpdate(float delta) {
            Internal.Info($"Player.OnUpdate: {delta}");

            const float speed = 1.0f;
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y += 1.0f;
            if (Input.IsKeyDown(KeyCode.S))
                velocity.Y -= 1.0f;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.X -= 1.0f;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X += 1.0f;

            Vector3 position = Position;
            position += velocity * delta;
            Position = position;
        }

        void OnDestroy() {
            Internal.Info("Player.OnDestroy");
        }
    }
}