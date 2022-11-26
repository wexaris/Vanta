using System;

namespace Vanta {

    public struct Vector2 {
        public float X, Y;

        public static Vector2 Zero => new Vector2(0.0f);

        public Vector2(float scalar) {
            X = scalar;
            Y = scalar;
        }

        public Vector2(float x, float y) {
            X = x;
            Y = y;
        }

        public static Vector2 operator +(Vector2 vec1, Vector2 vec2) {
            return new Vector2(vec1.X + vec2.X, vec1.Y + vec2.Y);
        }

        public static Vector2 operator -(Vector2 vec1, Vector2 vec2) {
            return new Vector2(vec1.X - vec2.X, vec1.Y - vec2.Y);
        }

        public static Vector2 operator *(Vector2 vec, float scalar) {
            return new Vector2(vec.X * scalar, vec.Y * scalar);
        }

        public static Vector2 operator /(Vector2 vec, float scalar) {
            return new Vector2(vec.X / scalar, vec.Y / scalar);
        }
    }

    public struct Vector3 {
        public float X, Y, Z;

        public Vector2 XY {
            get => new Vector2(X, Y);
            set {
                X = value.X;
                Y = value.Y;
            }
        }

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

        public static Vector3 operator +(Vector3 vec1, Vector3 vec2) {
            return new Vector3(vec1.X + vec2.X, vec1.Y + vec2.Y, vec1.Z + vec2.Z);
        }

        public static Vector3 operator -(Vector3 vec1, Vector3 vec2) {
            return new Vector3(vec1.X - vec2.X, vec1.Y - vec2.Y, vec1.Z - vec2.Z);
        }

        public static Vector3 operator *(Vector3 vec, float scalar) {
            return new Vector3(vec.X * scalar, vec.Y * scalar, vec.Z * scalar);
        }

        public static Vector3 operator /(Vector3 vec, float scalar) {
            return new Vector3(vec.X / scalar, vec.Y / scalar, vec.Z / scalar);
        }
    }
}