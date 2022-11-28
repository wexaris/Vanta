using System;

using Vanta;

namespace Sandbox {

    public class Camera : Entity {
        
        public float DistanceZ = 5.0f;

        private Entity Player;

        void OnCreate() {
            Position = new Vector3(Position.XY, DistanceZ);

            Player = GetEntityByName("Player");
            if (!Player) {
                Log.Error("Failed to find player entity!");
            }
        }

        void OnUpdate(float ts) {
            if (Player)
                Position = new Vector3(Player.Position.XY, DistanceZ);
        }
    }
}