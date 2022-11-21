using System;
using System.Runtime.CompilerServices;

namespace Vanta {

    public struct UUID { internal UInt64 Value; }

    public class Entity {
        protected UUID EntityID;

        public Vector3 Position {
            get {
                Internal.Entity_GetPosition(EntityID, out Vector3 position);
                return position;
            }
            set {
                Internal.Entity_SetPosition(EntityID, ref value);
            }
        }

        protected Entity() { EntityID = new UUID { Value = 0 }; }

        internal Entity(UUID id) {
            EntityID = id;
        }
    }

    public struct Vector3 {
        public float X, Y, Z;

        public static Vector3 Zero => new Vector3(0.0f);

        public Vector3(float scalar) {
            X = scalar;
            Y = scalar;
            Z = scalar;
        }

        public Vector3(float x, float y, float z) {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vector3 operator+(Vector3 vec1, Vector3 vec2) {
            return new Vector3(vec1.X + vec2.X, vec1.Y + vec2.Y, vec1.Z + vec2.Z);
        }

        public static Vector3 operator*(Vector3 vec, float scalar) {
            return new Vector3(vec.X * scalar, vec.Y * scalar, vec.Z * scalar);
        }
    }

    public static class Input {
        public static bool IsKeyDown(KeyCode key) {
            return Internal.Input_IsKeyDown(key);
        }
    }

    public static class Internal {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Trace(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Info(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Warn(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Error(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Critical(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetPosition(UUID entityID, out Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetPosition(UUID entityID, ref Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode key);
    }
}
