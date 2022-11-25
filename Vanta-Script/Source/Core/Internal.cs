using System;
using System.Runtime.CompilerServices;

namespace Vanta {

    public static class Internal {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Trace(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Info(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Warn(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Error(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Critical(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode key);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsMouseDown(MouseButton button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(UUID entityID, Type type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetPosition(UUID entityID, out Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetPosition(UUID entityID, ref Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, ref Vector2 impulse, bool wake);
    }
}