using System;

using Vanta;

namespace Sandbox
{
    public class Player : Entity
    {
        void OnCreate()
        {
            Internal.Info("Player.OnCreate");
        }

        void OnUpdate(float delta)
        {
            Internal.Info($"Player.OnUpdate: {delta}");
        }

        void OnDestroy()
        {
            Internal.Info("Player.OnDestroy");
        }
    }
}