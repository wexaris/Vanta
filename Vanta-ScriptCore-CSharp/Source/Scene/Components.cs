
namespace Vanta {

    public abstract class Component {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component {

        public Vector3 Position {
            get {
                Internal.TransformComponent_GetPosition(Entity.ID, out Vector3 position);
                return position;
            }
            set {
                Internal.TransformComponent_SetPosition(Entity.ID, ref value);
            }
        }
    }

    public class Rigidbody2DComponent : Component {
        
        public void ApplyLinearImpulse(Vector2 impulse, bool wake = true) {
            Internal.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }

    public class BoxCollider2DComponent : Component {}
    public class CircleCollider2DComponent : Component {}

    public class SpriteComponent : Component { }
    public class CircleRendererComponent : Component { }

    public class CameraComponent : Component {}

    public class ScriptComponent : Component {}
    public class NativeScriptComponent : Component {}
}