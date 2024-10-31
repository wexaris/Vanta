#pragma once
#include "Vanta/Scene/Components.hpp"

namespace Vanta {

    usize GetBufferIndex();
    void SetBufferIndex(usize);

    template<typename Base_t>
    struct Buffered {
    public:
        using Base = Base_t;

        Buffered() = default;
        Buffered(const Buffered&) = default;

        Base& Get() { return GetBufferIndex() == 0 ? m_First : m_Second; }
        Base& Set() { return GetBufferIndex() == 0 ? m_Second : m_First; }

        operator Base* () { return &Get(); }
        operator Base& () { return Get(); }

    private:
        Base m_First = Base();
        Base m_Second = Base();
    };

    /// <summary>
    /// Scene registry with support for multiple buffered components.
    /// Allows for iteration over entities, while automatically resolving the
    /// current buffer instance.
    /// Use `View()` to iterate over entities.
    /// Use `SwapBuffers()` to go to the next buffer instance.
    /// </summary>
    template<typename... ToBuffer>
    class BufferedRegistry {
    private:
        static constexpr uint SIZE = 2;

        /// MapIf
        template<template<typename> typename Cond, template<typename> typename Map, typename... Type>
        struct MapIf {
            // ComponentList<MappedTypes...>
            using type = ComponentList<typename std::conditional_t<Cond<Type>::value, typename Map<Type>::type, Type>...>;
        };

        template<template<typename> typename Cond, template<typename> typename Map, typename... Type>
        using MapIf_t = MapIf<Cond, Map, Type...>::type;

        /// IsBuffered
        template<typename T>
        struct IsBuffered {
            static constexpr bool value = (std::is_same_v<T, ToBuffer> || ...);
        };

        template<typename Base>
        static constexpr bool IsBuffered_v = IsBuffered<Base>::value;

        /// Resolved
        template<typename... Components>
        struct Resolved {
            template<typename Base>
            struct Buffer { using type = Buffered<Base>; };

            // ComponentList<Components...>
            using type = MapIf_t<IsBuffered, Buffer, Components...>;
        };

        template<typename... Components>
        using Resolved_t = Resolved<Components...>::type;

    public:
        BufferedRegistry() = default;

        /// <summary>
        /// Swap buffers by incrementing the current buffer index.
        /// </summary>
        void SwapBuffers() {
            SetBufferIndex((GetBufferIndex() + 1) % SIZE);
        }

        /// <summary>
        /// Swap buffers by copying the next one's data to the current one.
        /// Slower than `SwapBuffers()` but not prone to jitter in editor interactions.
        /// </summary>
        void SwapBuffersFwd() {
            ((View<ToBuffer>([](auto, Buffered<ToBuffer>& buffer) {
                auto& get = buffer.Get();
                auto set = buffer.Set();
                get = std::move(set);
            })), ...);
        }

        /// <summary>
        /// Create a new entity.
        /// </summary>
        entt::entity Create() {
            return m_Registry.create();
        }

        /// <summary>
        /// Destroy an entity.
        /// </summary>
        void Destroy(entt::entity entity) {
            m_Registry.destroy(entity);
        }

        /// <summary>
        /// Execute a function on each entity.
        /// </summary>
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

        /// <summary>
        /// Get an entities component.
        /// </summary>
        template<typename Component>
        decltype(auto) GetComponent(entt::entity entity) {
            return Impl_Get<Resolved_t<Component>>::Exec(m_Registry, entity);
        }

        /// <summary>
        /// Try to get an entities component.
        /// </summary>
        template<typename Component>
        decltype(auto) TryGetComponent(entt::entity entity) {
            return Impl_TryGet<Resolved_t<Component>>::Exec(m_Registry, entity);
        }

        /// <summary>
        /// Execute a function on each entity that has the given components.
        /// 
        /// Expected function parameters: (entity, component1, component2, ...)
        /// </summary>
        template<typename... Components, typename Func>
        void View(Func&& func) {
            Impl_View<Resolved_t<Components...>>::Exec(m_Registry, std::forward<Func>(func));
        }

        template<typename... Components>
        decltype(auto) View() {
            return Impl_View<Resolved_t<Components...>>::Exec(m_Registry);
        }

        /// <summary>
        /// Get an iterator over entities that have the given components.
        /// 
        /// Expected function parameters: (view, iter_beg, inter_end)
        /// </summary>
        template<typename... Components, typename Func>
        void ViewIter(Func&& func) {
            Impl_ViewIter<Resolved_t<Components...>>::Exec(m_Registry, std::forward<Func>(func));
        }

        /// <summary>
        /// Add a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args>
        decltype(auto) AddComponent(entt::entity entity, Args&&... args) {
            return Impl_Emplace<Resolved_t<Component>>::Exec(m_Registry, entity, args...);
        }

        /// <summary>
        /// Add or replace a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args>
        decltype(auto) AddOrReplaceComponent(entt::entity entity, Args&&... args) {
            return Impl_EmplaceOrReplace<Resolved_t<Component>>::Exec(m_Registry, entity, args...);
        }

        /// <summary>
        /// Remove a given component from an entity.
        /// </summary>
        template<typename Component>
        void RemoveComponent(entt::entity entity) {
            Impl_Remove<Resolved_t<Component>>::Exec(m_Registry, entity);
        }

        /// <summary>
        /// Check if an entity has a certain component.
        /// </summary>
        template<typename... Components>
        bool HasComponent(entt::entity entity) const {
            return Impl_AnyOf<Resolved_t<Components...>>::Exec(m_Registry, entity);
        }

        /// <summary>
        /// Check if an entity is valid.
        /// </summary>
        bool IsValid(entt::entity entity) const {
            return m_Registry.valid(entity);
        }

    private:
        template<typename...>
        friend struct BufferedRegistryBuilder;

        entt::registry m_Registry;

        /// <summary>
        /// Get
        /// </summary>
        template<typename... Components>
        struct Impl_Get {
            static decltype(auto) Exec(entt::registry& registry, entt::entity e) {
                return registry.get<Components...>(e);
            }

            static decltype(auto) Exec(const entt::registry& registry, entt::entity e) {
                return registry.get<Components...>(e);
            }
        };

        template<typename... Components>
        struct Impl_Get<ComponentList<Components...>> : public Impl_Get<Components...> {};

        /// <summary>
        /// TryGet
        /// </summary>
        template<typename... Components>
        struct Impl_TryGet {
            static decltype(auto) Exec(entt::registry& registry, entt::entity e) {
                return registry.try_get<Components...>(e);
            }

            static decltype(auto) Exec(const entt::registry& registry, entt::entity e) {
                return registry.try_get<Components...>(e);
            }
        };

        template<typename... Components>
        struct Impl_TryGet<ComponentList<Components...>> : public Impl_TryGet<Components...> {};

        /// <summary>
        /// View
        /// </summary>
        template<typename... Components>
        struct Impl_View {
            static decltype(auto) Exec(entt::registry& registry) {
                return registry.view<Components...>();
            }

            static decltype(auto) Exec(const entt::registry& registry) {
                return registry.view<Components...>();
            }

            template<typename Func>
            static void Exec(entt::registry& registry, Func&& func) {
                auto view = registry.view<Components...>();
                view.each(func);
            }

            template<typename Func>
            static void Exec(const entt::registry& registry, Func&& func) {
                auto view = registry.view<Components...>();
                view.each(func);
            }
        };

        template<typename... Components>
        struct Impl_View<ComponentList<Components...>> : public Impl_View<Components...> {};

        /// <summary>
        /// ViewIter
        /// </summary>
        template<typename... Components>
        struct Impl_ViewIter {
            template<typename Func>
            static void Exec(entt::registry& registry, Func&& func) {
                auto view = registry.view<Components...>();
                auto each = view.each();

                auto beg = each.begin();
                auto end = each.end();

                func(view, beg, end);
            }

            template<typename Func>
            static void Exec(const entt::registry& registry, Func&& func) {
                auto view = registry.view<Components...>();
                auto each = view.each();

                auto beg = each.begin();
                auto end = each.end();

                func(view, beg, end);
            }
        };

        template<typename... Components>
        struct Impl_ViewIter<ComponentList<Components...>> : public Impl_ViewIter<Components...> {};

        /// <summary>
        /// Emplace
        /// </summary>
        template<typename... Components>
        struct Impl_Emplace {
            template<typename... Args>
            static void Exec(entt::registry& registry, entt::entity e, Args&&... args) {
                ((Impl_Emplace<Components>::Exec(registry, e, args...)), ...);
            }
        };

        template<typename Component>
        struct Impl_Emplace<Component> {
            template<typename... Args>
            static decltype(auto) Exec(entt::registry& registry, entt::entity e, Args&&... args) {
                return registry.emplace<Component>(e, args...);
            }
        };

        template<typename... Components>
        struct Impl_Emplace<ComponentList<Components...>> : public Impl_Emplace<Components...> {};

        /// <summary>
        /// EmplaceOrReplace
        /// </summary>
        template<typename... Components>
        struct Impl_EmplaceOrReplace {
            template<typename... Args>
            static void Exec(entt::registry& registry, entt::entity e, Args&&... args) {
                ((Impl_EmplaceOrReplace<Components>::Exec(registry, e, args...)), ...);
            }
        };

        template<typename Component>
        struct Impl_EmplaceOrReplace<Component> {
            template<typename... Args>
            static decltype(auto) Exec(entt::registry& registry, entt::entity e, Args&&... args) {
                return registry.emplace_or_replace<Component>(e, args...);
            }
        };

        template<typename... Components>
        struct Impl_EmplaceOrReplace<ComponentList<Components...>> : public Impl_EmplaceOrReplace<Components...> {};

        /// <summary>
        /// Remove
        /// </summary>
        template<typename... Components>
        struct Impl_Remove {
            template<typename... Args>
            static void Exec(entt::registry& registry, entt::entity e, Args&&... args) {
                ((registry.remove<Components>(e, args...)), ...);
            }
        };

        template<typename... Components>
        struct Impl_Remove<ComponentList<Components...>> : public Impl_Remove<Components...> {};

        /// <summary>
        /// Any Of
        /// </summary>
        template<typename... Components>
        struct Impl_AnyOf {
            static bool Exec(const entt::registry& registry, entt::entity e) {
                return registry.any_of<Components...>(e);
            }
        };

        template<typename... Components>
        struct Impl_AnyOf<ComponentList<Components...>> : public Impl_AnyOf<Components...> {};
    };
}
