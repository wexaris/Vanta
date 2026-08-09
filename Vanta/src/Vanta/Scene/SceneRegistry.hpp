#pragma once

#include "Vanta/Scene/Components.hpp"

#include <utility>

namespace Vanta {

    /// <summary>
    /// Scene registry facade over entt::registry.
    ///
    /// This is the canonical component store for Scene state.
    /// Deferred mutation ordering is handled by command queues rather than
    /// component-level buffering.
    /// </summary>
    class SceneRegistry {
    public:
        SceneRegistry() = default;

        entt::entity Create() {
            return m_Registry.create();
        }

        void Destroy(entt::entity entity) {
            m_Registry.destroy(entity);
        }

        template<typename Func>
        void Each(Func&& func) {
            for (auto& entity : m_Registry.view<entt::entity>()) {
                func(entity);
            }
        }

        template<typename Func>
        void Each(Func&& func) const {
            for (auto& entity : m_Registry.view<entt::entity>()) {
                func(entity);
            }
        }

        template<typename Component>
        decltype(auto) GetComponent(entt::entity entity) {
            return m_Registry.get<Component>(entity);
        }

        template<typename Component>
        decltype(auto) GetComponent(entt::entity entity) const {
            return m_Registry.get<Component>(entity);
        }

        template<typename Component>
        decltype(auto) TryGetComponent(entt::entity entity) {
            return m_Registry.try_get<Component>(entity);
        }

        template<typename Component>
        decltype(auto) TryGetComponent(entt::entity entity) const {
            return m_Registry.try_get<Component>(entity);
        }

        template<typename... Components, typename Func>
        void View(Func&& func) {
            auto view = m_Registry.view<Components...>();
            view.each(std::forward<Func>(func));
        }

        template<typename... Components>
        decltype(auto) View() {
            return m_Registry.view<Components...>();
        }

        template<typename... Components>
        decltype(auto) View() const {
            return m_Registry.view<Components...>();
        }

        template<typename... Components, typename Func>
        void ViewIter(Func&& func) {
            auto view = m_Registry.view<Components...>();
            auto each = view.each();

            auto beg = each.begin();
            auto end = each.end();

            func(view, beg, end);
        }

        template<typename... Components, typename Func>
        void ViewIter(Func&& func) const {
            auto view = m_Registry.view<Components...>();
            auto each = view.each();

            auto beg = each.begin();
            auto end = each.end();

            func(view, beg, end);
        }

        template<typename Component, typename... Args>
        decltype(auto) AddComponent(entt::entity entity, Args&&... args) {
            return m_Registry.emplace<Component>(entity, std::forward<Args>(args)...);
        }

        template<typename Component, typename... Args>
        decltype(auto) AddOrReplaceComponent(entt::entity entity, Args&&... args) {
            return m_Registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
        }

        template<typename Component>
        void RemoveComponent(entt::entity entity) {
            m_Registry.remove<Component>(entity);
        }

        template<typename... Components>
        bool HasComponent(entt::entity entity) const {
            return m_Registry.any_of<Components...>(entity);
        }

        bool IsValid(entt::entity entity) const {
            return m_Registry.valid(entity);
        }

        entt::registry& Raw() { return m_Registry; }
        const entt::registry& Raw() const { return m_Registry; }

    private:
        entt::registry m_Registry;
    };

} // namespace Vanta