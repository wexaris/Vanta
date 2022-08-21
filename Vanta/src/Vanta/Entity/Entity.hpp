#pragma once
#include "Vanta/Entity/Scene.hpp"
#include "Vanta/Event/Event.hpp"

#include <entt/entt.hpp>

namespace Vanta {

    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;
        virtual ~Entity() = default;

        virtual void OnEvent(Event&) {}

        /// Add or replace a component of a certain type.
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            VANTA_ASSERT(!HasComponent<T>(), "Entity already has component");
            T& component = m_Scene->GetRegistry().emplace_or_replace<T>(m_Handle, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded(*this, component);
            return component;
        }

        template<typename T>
        T& GetComponent() {
            VANTA_ASSERT(HasComponent<T>(), "Entity does not have component");
            return m_Scene->GetRegistry().get<T>(m_Handle);
        }

        template<typename T>
        bool HasComponent() {
            return m_Scene->GetRegistry().any_of<T>(m_Handle);
        }

        template<typename T>
        void RemoveComponent() {
            VANTA_ASSERT(HasComponent<T>(), "Entity does not have component");
            return m_Scene->GetRegistry().remove<T>(m_Handle);
        }

        const std::string& GetName();

        operator entt::entity() const { return m_Handle; }

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
