#pragma once
#include "Vanta/Scene/Components.hpp"

namespace Vanta {

    template<typename Component>
    struct BufferedComponent {
        BufferedComponent(std::tuple<Component*, Component*> components)
            : m_Curr(std::get<0>(components)), m_Next(std::get<1>(components)) {}

        BufferedComponent(std::tuple<Component&, Component&> components)
            : m_Curr(&std::get<0>(components)), m_Next(&std::get<1>(components)) {}

        Component& Get() { return *m_Curr; }
        Component& Set() { return *m_Next; }

        operator Component*() { return m_Curr; }
        operator Component&() { return *m_Curr; }

    private:
        Component* m_Curr = nullptr;
        Component* m_Next = nullptr;
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

        template<typename Base_t>
        struct Buffered {
            struct Buffer_1 : public Base_t {};
            struct Buffer_2 : public Base_t {};

            using Base = Base_t;
            using List = ComponentList<Buffer_1, Buffer_2>;
        };

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

        /// IsBuffer
        template<typename T>
        struct IsBuffer {
            static constexpr bool value = (Contains_v<T, typename Buffered<ToBuffer>::List> || ...);
        };

        template<typename Base>
        static constexpr bool IsBuffer_v = IsBuffer<Base>::value;

        /// BaseOf
        template<typename T>
        struct BaseOf {
            template<typename Item>
            using MatchBuffer = Contains<T, typename Item::List>;

            using type = Find_t<MatchBuffer, Buffered<ToBuffer>...>::Base;
        };

        template<typename Buffer>
        using BaseOf_t = BaseOf<Buffer>::type;

        /// BuffersOf
        template<typename T>
        using BuffersOf = std::conditional_t<IsBuffered_v<T>, typename Buffered<T>::List, T>;

        template<typename T>
        using BuffersOf_t = BuffersOf<T>;

        /// ResolvedComponents
        template<usize Index, typename... Components>
        struct Resolved {
            template<usize, typename Base>
            struct Resolve { using type = Base; };

            template<usize Index, typename Base> requires IsBuffered_v<Base>
            struct Resolve<Index, Base> : public Get<Index, BuffersOf_t<Base>> {};

            template<typename Base>
            using Resolve_x = Resolve<Index, Base>;

            // ComponentList<Components...>
            using type = MapIf_t<IsBuffered, Resolve_x, Components...>;
        };

        template<typename... Components>
        using Resolved_0 = Resolved<0, Components...>::type;

        template<typename... Components>
        using Resolved_1 = Resolved<1, Components...>::type;

    public:
        BufferedRegistry() = default;

        /// <summary>
        /// Swap to the next buffer of any buffered components.
        /// </summary>
        void SwapBuffers() {
            m_BufferIdx = (m_BufferIdx + 1) % SIZE;
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
            return m_Registry.each(func);
        }

        template<typename Func>
        void Each(Func&& func) const {
            return m_Registry.each(func);
        }

        /// <summary>
        /// Get an entities component.
        /// </summary>
        template<typename Component> requires !IsBuffered_v<Component>
        Component& GetComponent(entt::entity entity) {
            return Impl_Get<Component>::Exec(m_Registry, entity);
        }

        template<typename Component> requires IsBuffered_v<Component>
        BufferedComponent<Component> GetComponent(entt::entity entity) {
            auto[b1, b2] = Impl_Get<BuffersOf_t<Component>>::Exec(m_Registry, entity);
            switch (m_BufferIdx) {
            case 0: return BufferedComponent<Component>({ b1, b2 });
            case 1: return BufferedComponent<Component>({ b2, b1 });
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                return BufferedComponent<Component>({ nullptr, nullptr });
            }
        }

        /// <summary>
        /// Try to get an entities component.
        /// </summary>
        template<typename Component>
        Component* TryGetComponent(entt::entity entity) {
            switch (m_BufferIdx) {
            case 0: return &Impl_TryGet<Resolved_0<Component>>::Exec(m_Registry, entity);
            case 1: return &Impl_TryGet<Resolved_1<Component>>::Exec(m_Registry, entity);
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                return nullptr;
            }
        }

        template<typename Component>
        const Component* TryGetComponent(entt::entity entity) const {
            switch (m_BufferIdx) {
            case 0: return Impl_TryGet<Resolved_0<Component>>::Exec(m_Registry, entity);
            case 1: return Impl_TryGet<Resolved_1<Component>>::Exec(m_Registry, entity);
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                return nullptr;
            }
        }

        /// <summary>
        /// Execute a function on each entity that has the given components.
        /// 
        /// Expected function parameters: (entity, component1, component2, ...)
        /// </summary>
        template<typename... Components, typename Func>
        void View(Func&& func) {
            return View<0, Components...>(std::forward<Func>(func));
        }

        template<usize Offset, typename... Components, typename Func>
        void View(Func&& func) {
            switch ((m_BufferIdx + Offset) % SIZE) {
            case 0:
                Impl_View<Resolved_0<Components...>>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_View<Resolved_1<Components...>>::Exec(m_Registry, std::forward<Func>(func));
                break;
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                break;
            }
        }

        template<typename... Components, typename Func>
        void View(Func&& func) const {
            return View<0, Components...>(std::forward<Func>(func));
        }

        template<usize Offset, typename... Components, typename Func>
        void View(Func&& func) const {
            switch ((m_BufferIdx + Offset) % SIZE) {
            case 0:
                Impl_View<Resolved_0<Components...>>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_View<Resolved_1<Components...>>::Exec(m_Registry, std::forward<Func>(func));
                break;
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                break;
            }
        }

        /// <summary>
        /// Get an iterator over entities that have the given components.
        /// 
        /// Expected function parameters: (view, iter_beg, inter_end)
        /// </summary>
        template<typename... Components, typename Func>
        void ViewIter(Func&& func) {
            switch (m_BufferIdx) {
            case 0:
                Impl_ViewIter<Resolved_0<Components...>>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_ViewIter<Resolved_1<Components...>>::Exec(m_Registry, std::forward<Func>(func));
                break;
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                break;
            }
        }

        template<typename... Components, typename Func>
        void ViewIter(Func&& func) const {
            switch (m_BufferIdx) {
            case 0:
                Impl_ViewIter<Resolved_0<Components...>>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_ViewIter<Resolved_1<Components...>>::Exec(m_Registry, std::forward<Func>(func));
                break;
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                break;
            }
        }

        /// <summary>
        /// Add a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args> requires !IsBuffered_v<Component>
        Component& AddComponent(entt::entity entity, Args&&... args) {
            return Impl_Emplace<Component>::Exec(m_Registry, entity, args...);
        }

        template<typename Component, typename... Args> requires IsBuffered_v<Component>
        BufferedComponent<Component> AddComponent(entt::entity entity, Args&&... args) {
            Impl_Emplace<BuffersOf_t<Component>>::Exec(m_Registry, entity, args...);
            return GetComponent<Component>(entity);
        }

        /// <summary>
        /// Add or replace a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args> requires !IsBuffered_v<Component>
        Component& AddOrReplaceComponent(entt::entity entity, Args&&... args) {
            return Impl_EmplaceOrReplace<Component>::Exec(m_Registry, entity, args...);
        }

        template<typename Component, typename... Args> requires IsBuffered_v<Component>
        Component& AddOrReplaceComponent(entt::entity entity, Args&&... args) {
            Impl_EmplaceOrReplace<BuffersOf_t<Component>>::Exec(m_Registry, entity, args...);
            return GetComponent<Component>(entity);
        }

        /// <summary>
        /// Remove a given component from an entity.
        /// </summary>
        template<typename Component> requires !IsBuffered_v<Component>
        void RemoveComponent(entt::entity entity) {
            Impl_Remove<Component>::Exec(m_Registry, entity);
        }

        template<typename Component> requires IsBuffered_v<Component>
        void RemoveComponent(entt::entity entity) {
            Impl_Remove<BuffersOf_t<Component>>::Exec(m_Registry, entity);
        }

        /// <summary>
        /// Check if an entity has a certain component.
        /// </summary>
        template<typename... Components>
        bool HasComponent(entt::entity entity) const {
            return Impl_AnyOf<Resolved_0<Components...>::type>::Exec(m_Registry, entity);
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
        usize m_BufferIdx = 0;

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
        struct Impl_TryGet<ComponentList<Components...>> : public Impl_Get<Components...> {};

        /// <summary>
        /// View
        /// </summary>
        template<typename... Components>
        struct Impl_View {
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
