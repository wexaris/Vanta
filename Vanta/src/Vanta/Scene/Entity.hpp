#pragma once
#include "Vanta/Event/Event.hpp"
#include "Vanta/Scene/Scene.hpp"

#include <entt/entt.hpp>

namespace Vanta {

    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;
        virtual ~Entity() = default;

        bool IsValid() const {
            return m_Scene ? m_Scene->IsValid(*this) : false;
        }

        /// Add a component of a certain type.
        template<typename Component, typename... Args>
        Component& AddComponent(Args&&... args) {
            return m_Scene->AddComponent<Component>(*this, std::forward<Args>(args)...);
        }

        /// Add or replace a component of a certain type.
        template<typename Component, typename... Args>
        Component& AddOrReplaceComponent(Args&&... args) {
            return m_Scene->AddOrReplaceComponent<Component>(*this, std::forward<Args>(args)...);
        }

        /// Remove a component of a certain type.
        template<typename Component>
        void RemoveComponent() {
            return m_Scene->RemoveComponent<Component>(*this);
        }

        /// Get a component of a certain type.
        template<typename Component>
        decltype(auto) GetComponent() {
            return m_Scene->GetComponent<Component>(*this);
        }

        template<typename Component>
        decltype(auto) GetComponent() const {
            return m_Scene->GetComponent<Component>(*this);
        }

        /// Check if entity has a component of a certain type.
        template<typename Component>
        bool HasComponent() const {
            VANTA_ASSERT(IsValid(), "Attempting to access invalid entity!");
            return m_Scene->HasComponent<Component>(*this);
        }

        UUID GetUUID() const;
        const std::string& GetName() const;

        entt::entity GetHandle() const { return m_Handle; }

        virtual void OnEvent(Event&) {}

        operator bool() const         { return IsValid(); }
        operator entt::entity() const { return GetHandle(); }
        operator uint32() const       { return (uint32)GetHandle(); }

        bool operator==(const Entity& other) const {
            return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
        }
        bool operator!=(const Entity& other) const {
            return m_Handle != other.m_Handle || m_Scene != other.m_Scene;
        }

    private:
        entt::entity m_Handle = entt::null;
        Scene* m_Scene = nullptr;
    };
}
