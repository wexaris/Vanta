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

        virtual void OnEvent(Event&) {}

        /// Add or replace a component of a certain type.
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            VANTA_ASSERT(!HasComponent<T>(), "Entity already has component: {}", typeid(T).name());
            return m_Scene->AddComponent<T>(*this, std::forward<Args>(args)...);
        }

        /// Remove a component of a certain type.
        template<typename T>
        void RemoveComponent() {
            VANTA_ASSERT(HasComponent<T>(), "Entity does not have component: {}", typeid(T).name());
            return m_Scene->RemoveComponent<T>(*this);
        }

        /// Get a component of a certain type.
        template<typename T>
        T& GetComponent() {
            VANTA_ASSERT(HasComponent<T>(), "Entity does not have component: {}", typeid(T).name());
            return m_Scene->GetComponent<T>(*this);
        }

        template<typename T>
        bool HasComponent() {
            return m_Scene->HasComponent<T>(*this);
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
