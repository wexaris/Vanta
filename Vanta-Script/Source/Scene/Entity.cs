using System;

namespace Vanta {

    public struct UUID {
        internal UInt64 Value;

        public static implicit operator bool(UUID id) {
            return id.Value != 0;
        }
    }

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

        public static implicit operator bool(Entity entity) {
            return !object.ReferenceEquals(entity, null) && entity.ID;
        }

        public T As<T>() where T : Entity, new() {
            object instance = Internal.Entity_GetScriptInstance(ID);
            return instance as T;
        }

        public Entity GetEntityByName(string name) {
            UUID entityID = Internal.Entity_GetEntityByName(name);
            if (!entityID)
                return null;
            return new Entity(entityID);
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
