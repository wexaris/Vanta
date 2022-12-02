#pragma once
#include "../Math/Math.hpp"

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

    VANTA_COMPONENT_REGISTER(TransformComponent)
    
}
