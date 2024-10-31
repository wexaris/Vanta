#pragma once
#include <Vanta/Vanta.hpp>
#include <Vanta/Math/Vector.hpp>

#define VANTA_COMPONENT_DEFINE(name) \
    public: \
        static ::Vanta::usize GetComponentID(); \
        static bool CONCAT(_register_, name); \
        name() = default; \
        name(const name&) = default; \
        name(::Vanta::UUID entityID) : ::Vanta::Component(entityID) {}

#define VANTA_COMPONENT_REGISTER(name) \
    ::Vanta::usize name::GetComponentID() { return typeid(name).hash_code(); } \
    bool name::CONCAT(_register_, name) = ::Vanta::Native::Registry::RegisterComponent(#name, name::GetComponentID());

namespace Vanta {

    struct Component {
    public:
        operator bool() const { return m_EntityID != 0; }

    protected:
        UUID m_EntityID = 0;

        Component() = default;
        Component(UUID entityID) : m_EntityID(entityID) {}
    };

    struct TransformComponent : public Component {
        VANTA_COMPONENT_DEFINE(TransformComponent);

        const Vector3& GetPosition() const {
            return Internal.TransformComponent_GetPosition(m_EntityID);
        }

        void SetPosition(const Vector3& value) {
            Internal.TransformComponent_SetPosition(m_EntityID, value);
        }
    };

    struct Rigidbody2DComponent : public Component {
        VANTA_COMPONENT_DEFINE(Rigidbody2DComponent);

        void ApplyLinearImpulse(const Vector2& value, bool wake = true) {
            return Internal.Rigidbody2DComponent_ApplyLinearImpulseToCenter(m_EntityID, value, wake);
        }
    };

    struct BoxCollider2DComponent : public Component {
        VANTA_COMPONENT_DEFINE(BoxCollider2DComponent);
    };

    struct CircleCollider2DComponent : public Component {
        VANTA_COMPONENT_DEFINE(CircleCollider2DComponent);
    };

    struct SpriteComponent : public Component {
        VANTA_COMPONENT_DEFINE(SpriteComponent);

        void SetColor(const Vector4& value) {
            return Internal.SpriteComponent_SetColor(m_EntityID, value);
        }
    };

    struct CircleRendererComponent : public Component {
        VANTA_COMPONENT_DEFINE(CircleRendererComponent);
    };

    struct CameraComponent : public Component {
        VANTA_COMPONENT_DEFINE(CameraComponent);
    };

    struct ScriptComponent : public Component {
        VANTA_COMPONENT_DEFINE(ScriptComponent);
    };

    struct NativeScriptComponent : public Component {
        VANTA_COMPONENT_DEFINE(NativeScriptComponent);
    };
}
