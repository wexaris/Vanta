#pragma once
#include "Vanta/Scene/Components.hpp"

namespace Vanta {

    /// <summary>
    /// Support type to be used by the BufferedRegistry.
    /// Maps the base and buffer instance types.
    /// </summary>
    template<typename Core, typename... Buffers>
    struct BufferedComponent {
        using Base = Core;
        using List = ComponentList<Buffers...>;

        static constexpr usize SIZE = sizeof...(Buffers);
    };

    template<typename Base, typename... Buffers>
    class BufferedComponent<Base, ComponentList<Buffers...>> : public BufferedComponent<Base, Buffers...> {};

    template<typename... Buffers>
    class SameSize;

    template<>
    class SameSize<> : public std::true_type {};

    template<typename Head>
    class SameSize<Head> : public std::true_type {};

    template<typename Head, typename... Tail>
    struct SameSize<Head, Tail...> {
        static constexpr bool value = ((Head::SIZE == Tail::SIZE) && ...);
    };

    template<typename... Buffers>
    constexpr bool SameSize_v = SameSize<Buffers...>::value;

    /// <summary>
    /// Scene registry with support for multiple buffered components.
    /// Allows for iteration over entities, while automatically resolving the
    /// current buffer instance.
    /// Use `View()` and `ViewNext()` to iterate over entities with the current or next instances of buffers.
    /// Use `SwapBuffers()` to go to the next buffer instance.
    /// </summary>
    template<typename... Buffers> requires SameSize_v<Buffers...>
    class BufferedRegistry {
    private:
        template<template<typename> typename Cond, template<typename> typename Map, typename... Type>
        struct MapIf {
            // ComponentList<MappedTypes...>
            using type = ComponentList<typename std::conditional_t<Cond<Type>::value, typename Map<Type>::type, Type>...>;
        };

        template<template<typename> typename Cond, template<typename> typename Map, typename... Type>
        using MapIf_t = MapIf<Cond, Map, Type...>::type;

        template<typename T>
        struct IsBuffered {
            static constexpr bool value = (std::is_same_v<T, typename Buffers::Base> || ...);
        };

        template<typename Base>
        static constexpr bool IsBuffered_v = IsBuffered<Base>::value;

        template<typename Buffer>
        struct IsBuffer {
            static constexpr bool value = (Contains_v<Buffer, typename Buffers::List> || ...);
        };

        template<typename Base>
        static constexpr bool IsBuffer_v = IsBuffer<Base>::value;

        template<typename Buffer>
        struct BaseOf {
            template<typename Item>
            using MatchBuffer = Contains<typename Item::List, Buffer>;

            using type = Find_t<MatchBuffer, Buffers...>::Base;
        };

        template<typename Base>
        struct BuffersOf {
            template<typename Item>
            using MatchBase = std::is_same<typename Item::Base, Base>;

            // ComponentList<Buffers...>
            using type = Find_t<MatchBase, Buffers...>::List;
        };

        template<typename Base>
        using Buffer = BufferedComponent<Base, typename BuffersOf<Base>::type>;

        template<usize, typename Base>
        struct IndexedBuffer { using type = Base; };

        template<usize Index, typename Base> requires IsBuffered_v<Base>
        struct IndexedBuffer<Index, Base> : public Get<Index, typename BuffersOf<Base>::type> {};

        template<usize Index, typename... Components>
        struct IndexedComponents {
            template<typename Base>
            using IndexedBuffer = IndexedBuffer<Index, Base>;

            using type = MapIf_t<IsBuffered, IndexedBuffer, Components...>;
        };

        template<typename... Components>
        struct IndexedComponents_0 : public IndexedComponents<0, Components...> {};

        template<typename... Components>
        struct IndexedComponents_1 : public IndexedComponents<1, Components...> {};

    public:
        constexpr BufferedRegistry() = default;

        /// <summary>
        /// Swap to the next buffer of any buffered components.
        /// </summary>
        void SwapBuffers() {
            m_BufferIdx = (m_BufferIdx + 1) % Get_t<0, Buffers...>::SIZE;
        }

        /// <summary>
        /// Create a new entity.
        /// </summary>
        entt::entity Create() {
            return m_Registry.create();
        }

        /// <summary>
        /// Destory an entity.
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
        template<typename Component>
        Component& GetComponent(entt::entity entity) {
            // FIXME: Support arbitrary buffer sizes
            switch (m_BufferIdx) {
            case 0: return Impl_Get<IndexedComponents_0<Component>::type>::Exec(m_Registry, entity);
            case 1: return Impl_Get<IndexedComponents_1<Component>::type>::Exec(m_Registry, entity);
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                return Impl_Get<Component>::Exec(m_Registry, entity);
            }
        }

        template<typename Component>
        const Component& GetComponent(entt::entity entity) const {
            // FIXME: Support arbitrary buffer sizes
            switch (m_BufferIdx) {
            case 0: return Impl_Get<IndexedComponents_0<Component>::type>::Exec(m_Registry, entity);
            case 1: return Impl_Get<IndexedComponents_1<Component>::type>::Exec(m_Registry, entity);
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                return Impl_Get<Component>::Exec(m_Registry, entity);
            }
        }

        /// <summary>
        /// Try to get an entities component.
        /// </summary>
        template<typename Component>
        Component* TryGetComponent(entt::entity entity) {
            switch (m_BufferIdx) {
            case 0: return &Impl_TryGet<IndexedComponents_0<Component>::type>::Exec(m_Registry, entity);
            case 1: return &Impl_TryGet<IndexedComponents_1<Component>::type>::Exec(m_Registry, entity);
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                return nullptr;
            }
        }

        template<typename Component>
        Component* TryGetComponent(entt::entity entity) const {
            switch (m_BufferIdx) {
            case 0: return Impl_TryGet<IndexedComponents_0<Component>::type>::Exec(m_Registry, entity);
            case 1: return Impl_TryGet<IndexedComponents_1<Component>::type>::Exec(m_Registry, entity);
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
            switch (m_BufferIdx) {
            case 0:
                Impl_View<IndexedComponents_0<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_View<IndexedComponents_1<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                break;
            }
        }

        template<typename... Components, typename Func>
        void View(Func&& func) const {
            switch (m_BufferIdx) {
            case 0:
                Impl_View<IndexedComponents_0<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_View<IndexedComponents_1<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                break;
            }
        }

        /// <summary>
        /// Execute a function on each entity that has the given components.
        /// Uses the next buffer of any buffered components.
        /// 
        /// Expected function parameters: (entity, component1, component2, ...)
        /// </summary>
        template<typename... Components, typename Func>
        void ViewNext(Func&& func) {
            switch (m_BufferIdx) {
            case 0:
                Impl_View<IndexedComponents_1<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_View<IndexedComponents_0<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                break;
            }
        }

        template<typename... Components, typename Func>
        void ViewNext(Func&& func) const {
            switch (m_BufferIdx) {
            case 0:
                Impl_View<IndexedComponents_1<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1: {
                Impl_View<IndexedComponents_0<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            }
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
                Impl_ViewIter<IndexedComponents_0<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_ViewIter<IndexedComponents_1<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
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
                Impl_ViewIter<IndexedComponents_0<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            case 1:
                Impl_ViewIter<IndexedComponents_1<Components...>::type>::Exec(m_Registry, std::forward<Func>(func));
                break;
            default:
                VANTA_UNREACHABLE("Invalid buffer index; only two buffers are currently supported!");
                break;
            }
        }

        /// <summary>
        /// Add a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args>
        Component& AddComponent(entt::entity entity, Args&&... args) {
            if constexpr (IsBuffered_v<Component>) {
                Impl_Emplace<typename Buffer<Component>::List>::Exec(m_Registry, entity, args...);
                return GetComponent<Component>(entity);
            }
            else {
                return Impl_Emplace<Component>::Exec(m_Registry, entity, args...);
            }
        }

        /// <summary>
        /// Add or replace a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args>
        Component& AddOrReplaceComponent(entt::entity entity, Args&&... args) {
            if constexpr (IsBuffered_v<Component>) {
                Impl_EmplaceOrReplace<typename Buffer<Component>::List>::Exec(m_Registry, entity, args...);
                return GetComponent<Component>(entity);
            }
            else {
                return Impl_EmplaceOrReplace<Component>::Exec(m_Registry, entity, args...);
            }
        }

        /// <summary>
        /// Remove a given component from an entity.
        /// </summary>
        template<typename Component>
        void RemoveComponent(entt::entity entity) {
            if constexpr (IsBuffered_v<Component>) {
                Impl_Remove<typename Buffer<Component>::List>::Exec(m_Registry, entity);
            }
            else {
                Impl_Remove<Component>::Exec(m_Registry, entity);
            }
        }

        /// <summary>
        /// Check if an entity has a certain component.
        /// </summary>
        template<typename... Components>
        bool HasComponent(entt::entity entity) const {
            return Impl_AnyOf<IndexedComponents_0<Components...>::type>::Exec(m_Registry, entity);
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

        static constexpr usize BUFFER_COUNT = sizeof...(Buffers);
        static constexpr usize BUFFER_SIZE = BUFFER_COUNT > 0 ? Get_t<0, Buffers...>::SIZE : 0;

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
