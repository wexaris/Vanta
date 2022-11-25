using System;

namespace Vanta {

    public struct UUID { internal UInt64 Value; }

    public class Entity {
        internal readonly UUID ID;

        public Vector3 Position {
            get {
                Internal.TransformComponent_GetPosition(ID, out Vector3 position);
                return position;
            }
            set {
                Internal.TransformComponent_SetPosition(ID, ref value);
            }
        }

        protected Entity() { ID = new UUID { Value = 0 }; }

        internal Entity(UUID id) {
            ID = id;
        }

        public T GetComponent<T>() where T : Component, new() {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }

        public bool HasComponent<T>() where T: Component {
            Type type = typeof(T);
            return Internal.Entity_HasComponent(ID, type);
        }
    }
}
