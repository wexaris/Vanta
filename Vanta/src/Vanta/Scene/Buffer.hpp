#pragma once
#include <entt/entt.hpp>

#include <execution>

namespace Vanta {

    /// <summary>
    /// Helper type for grouping and shuttling components around.
    /// </summary>
    template<typename... Components>
    struct ComponentList {
        using types = entt::type_list<Components...>;

        static void Setup(entt::registry& registry) {
            (AttachTriggers<Components>(registry), ...);
        }

    private:
        /// <summary>
        /// Ensure that when one of the buffers is attached, all others are as well.
        /// </summary>
        template<typename Component>
        static void AttachTriggers(entt::registry& registry) {
            registry.on_construct<Component>().template connect<ComponentList::AttachAll>();
            registry.on_destroy<Component>().template connect<ComponentList::DetachAll>();
        }

        static void AttachAll(entt::registry& registry, entt::entity entity) {
            (Attach<Components>(registry, entity), ...);
        }

        static void DetachAll(entt::registry& registry, entt::entity entity) {
            (Detach<Components>(registry, entity), ...);
        }

        template<typename Component>
        static void Attach(entt::registry& registry, entt::entity entity) {
            if (!registry.any_of<Component>(entity))
                registry.emplace<Component>(entity);
        }

        template<typename Component>
        static void Detach(entt::registry& registry, entt::entity entity) {
            if (registry.any_of<Component>(entity))
                registry.remove<Component>(entity);
        }
    };

    template<usize N, typename... Types>
    struct Get<N, ComponentList<Types...>> : Get<N, Types...> {};

    template <typename T, typename... Types>
    struct Contains<T, ComponentList<Types...>> : Contains<Types...> {};

    namespace detail {

        template<typename Comp>
        class GetterDispatch {
        public:
            Comp* Component = nullptr;

            GetterDispatch(entt::entity entity)
                : m_Entity(entity) {}

            template<typename... Components>
            void operator()(const entt::view<entt::get_t<Components...>>& view) {
                Get_<0>(std::forward_as_tuple(view.template get<Components...>(m_Entity)));
            }

        private:
            entt::entity m_Entity;

            template<usize N, typename... Components> requires (N < sizeof...(Components))
                void Get_(const std::tuple<Components...>& components) {
                if constexpr (IsBase_v<Comp, RemoveRef_t<Get_t<N, Components...>>>) {
                    Component = &std::get<N>(components);
                }
                else {
                    Get_<N + 1>(components);
                }
            }

            template<usize N, typename... Components> requires (N >= sizeof...(Components))
                void Get_(const std::tuple<Components...>&) {
                VANTA_UNREACHABLE("Buffer index out of bounds; iterator {}/{}!", N, sizeof...(Components));
            }
        };
    }

    /// <summary>
    /// Dispatcher that iterates over a collection of entities in sequence,
    /// on the calling thread.
    /// </summary>
    /// <typeparam name="Functor">Functor to execute for every entity and its components.</typeparam>
    template<typename Functor>
    class LinearDispatch {
    public:
        template<typename... Args>
        LinearDispatch(Args&&... args)
            : m_Functor(std::forward<Args>(args)...) {}

        template<typename... Components>
        void operator()(const entt::view<entt::get_t<Components...>>& view) const {
            auto beg = view.begin();
            auto end = view.end();

            if (beg == end)
                return;

            auto func = [&](auto&&... args) {
                m_Functor(args...);
            };

            for (; beg != end; ++beg) {
                entt::entity entity = *beg;
                auto components = view.template get<Components...>(entity);
                auto args = std::tuple_cat(std::make_tuple(entity), components);
                std::apply(func, args);
            }
        }

    private:
        Functor m_Functor;

        LinearDispatch(LinearDispatch&&) = delete;
        LinearDispatch(const LinearDispatch&) = delete;
        LinearDispatch operator=(const LinearDispatch&) = delete;
    };

    /// <summary>
    /// Dispatcher that iterates over a collection of entities in parallel,
    /// on the engine's thread pool.
    /// </summary>
    /// <typeparam name="Functor">Functor to execute for every entity and its components.</typeparam>
    template<typename Functor>
    class ParalelDispatch {
    public:
        static constexpr usize CHUNK_SIZE = 8;

        template<typename... Args>
        ParalelDispatch(Args&&... args)
            : m_Functor(std::forward<Args>(args)...) {}

        template<typename... Components>
        void operator()(const entt::view<entt::get_t<Components...>>& view) const {
            auto beg = view.begin();
            auto end = view.end();

            if (beg == end)
                return;

            auto func = [&](auto&&... args) {
                m_Functor(args...);
            };

            usize taskCount = (usize)std::ceil((float)view.size_hint() / CHUNK_SIZE);

            if (taskCount > 1) {
                Fibers::Begin(taskCount);
                fibers::barrier bar(taskCount + 1); // + 1 for this spawner thread

                for (usize i = 0; i < taskCount; i++) {
                    Fibers::Spawn([&](auto beg) {
                        for (usize i = 0; i < CHUNK_SIZE && beg != end; ++i, ++beg) {
                            auto entity = *beg;
                            auto components = view.template get<Components...>(entity);
                            auto args = std::tuple_cat(std::make_tuple(entity), components);
                            std::apply(func, args);
                        }
                        bar.wait();
                    }, beg);

                    for (usize i = 0; i < CHUNK_SIZE && beg != end; ++i)
                        ++beg;
                }

                bar.wait();
                Fibers::End();
            }
            else {
                for (; beg != end; ++beg) {
                    auto entity = *beg;
                    auto components = view.template get<Components...>(entity);
                    auto args = std::tuple_cat(std::make_tuple(entity), components);
                    std::apply(func, args);
                }
            }
        }

    private:
        Functor m_Functor;

        ParalelDispatch(ParalelDispatch&&) = delete;
        ParalelDispatch(const ParalelDispatch&) = delete;
        ParalelDispatch operator=(const ParalelDispatch&) = delete;
    };

    template<typename... ComponentLists>
    class Buffer;

    template<typename... Components, typename... ComponentLists>
    class Buffer<ComponentList<Components...>, ComponentLists...> {
    public:
        Buffer() = default;

        static void Setup(entt::registry& registry) {
            ComponentList<Components...>::Setup(registry);
            (ComponentLists::Setup(registry), ...);
        }

        /// <summary>
        /// Run given function on the last and current data buffers.
        /// </summary>
        /// <typeparam name="Unbuffered">Other non-buffered types to retrieve.</typeparam>
        /// <param name="registry">Entity registry.</param>
        /// <param name="dispatcher">Dispatcher that will handle the view.</param>
        template<typename... Unbuffered, class Dispatcher>
        void View(entt::registry& registry, const Dispatcher& dispatcher) {
            ViewTwo_<0, Unbuffered...>(m_BufferIdx, registry, dispatcher);
        }

        template<typename... Unbuffered, class Dispatcher>
        void View(entt::registry& registry, Dispatcher& dispatcher) {
            ViewTwo_<0, Unbuffered...>(m_BufferIdx, registry, dispatcher);
        }

        /// <summary>
        /// Run given function on the current data buffer.
        /// </summary>
        /// <typeparam name="Unbuffered">Other non-buffered types to retrieve.</typeparam>
        /// <param name="registry">Entity registry.</param>
        /// <param name="dispatcher">Dispatcher that will handle the view.</param>
        template<typename... Unbuffered, class Dispatcher>
        void ViewCurr(entt::registry& registry, const Dispatcher& dispatcher) {
            ViewOne_<0, Unbuffered...>(m_BufferIdx, registry, dispatcher);
        }

        template<typename... Unbuffered, class Dispatcher>
        void ViewCurr(entt::registry& registry, Dispatcher& dispatcher) {
            ViewOne_<0, Unbuffered...>(m_BufferIdx, registry, dispatcher);
        }

        /// <summary>
        /// Run given function on the previous data buffer.
        /// </summary>
        /// <typeparam name="Unbuffered">Other non-buffered components to retrieve.</typeparam>
        /// <param name="registry">Entity registry.</param>
        /// <param name="dispatcher">Dispatcher that will handle the view.</param>
        template<typename... Unbuffered, class Dispatcher>
        void ViewPrev(entt::registry& registry, const Dispatcher& dispatcher) {
            ViewOne_<0, Unbuffered...>((m_BufferIdx - 1) % sizeof...(Components), registry, dispatcher);
        }

        template<typename... Unbuffered, class Dispatcher>
        void ViewPrev(entt::registry& registry, Dispatcher& dispatcher) {
            ViewOne_<0, Unbuffered...>((m_BufferIdx - 1) % sizeof...(Components), registry, dispatcher);
        }

        /// <summary>
        /// Get the current instance of a buffered component.
        /// </summary>
        template<typename Component>
        Component& Get(entt::registry& registry, entt::entity entity) {
            VANTA_ASSERT(HasComponent<Component>(), "Entity does not have component: {}", typeid(Component).name());
            detail::GetterDispatch<Component> getter(entity);
            ViewCurr(registry, getter);
            return *getter.Component;
        }

        template<typename Component>
        static constexpr bool HasComponent() {
            return Contains_v<Component, Components..., ComponentLists...>;
        }

        /// <summary>
        /// Switch to the next buffer.
        /// </summary>
        void Next() {
            m_BufferIdx = (m_BufferIdx + 1) % sizeof...(Components);
        }

    private:
        using FirstIdents = entt::identifier<Components...>;

        usize m_BufferIdx = 0; // Current buffer index

        /// <summary>
        /// Iterate through variadic params looking for the previous buffer.
        /// </summary>
        /// <typeparam name="N">Current iteration.</typeparam>
        /// <typeparam name="Unbuffered">Other non-buffered components to retrieve.</typeparam>
        /// <param name="registry">Entity registry.</param>
        /// <param name="dispatcher">Dispatcher that will handle the view.</param>
        template<usize N, typename... Unbuffered, class Dispatcher> requires (N < sizeof...(Components))
        static void ViewOne_(usize idx, entt::registry& registry, const Dispatcher& dispatcher) {
            if (idx == N) {
                auto view = registry.view<
                    Get_t<(N - 1) % sizeof...(Components), Components...>,
                    Get_t<(N - 1) % sizeof...(Components), ComponentLists>...,
                    Unbuffered...>();
                dispatcher(view);
            }
            else {
                ViewOne_<N + 1, Unbuffered...>(idx, registry, dispatcher);
            }
        }
        
        template<usize N, typename... Unbuffered, class Dispatcher> requires (N < sizeof...(Components))
        static void ViewOne_(usize idx, entt::registry& registry, Dispatcher& dispatcher) {
            if (idx == N) {
                auto view = registry.view<
                    Get_t<(N - 1) % sizeof...(Components), Components...>,
                    Get_t<(N - 1) % sizeof...(Components), ComponentLists>...,
                    Unbuffered...>();
                dispatcher(view);
            }
            else {
                ViewOne_<N + 1, Unbuffered...>(idx, registry, dispatcher);
            }
        }

        /// <summary>
        /// Iterate through variadic params looking for the last and current buffer.
        /// </summary>
        /// <typeparam name="N">Current iteration.</typeparam>
        /// <typeparam name="Unbuffered">Other non-buffered types to retrieve.</typeparam>
        /// <param name="registry">Entity registry.</param>
        /// <param name="dispatcher">Dispatcher that will handle the view.</param>
        template<usize N, typename... Unbuffered, class Dispatcher> requires (N < sizeof...(Components))
        static void ViewTwo_(usize idx, entt::registry& registry, const Dispatcher& dispatcher) {
            if (idx == N) {
                auto view = registry.view<
                    Get_t<(N - 1) % sizeof...(Components), Components...>,
                    Get_t<(N - 1) % sizeof...(Components), ComponentLists>...,
                    Get_t<N, Components...>,
                    Get_t<N, ComponentLists>...,
                    Unbuffered...>();
                dispatcher(view);
            }
            else {
                ViewTwo_<N + 1, Unbuffered...>(idx, registry, dispatcher);
            }
        }

        template<usize N, typename... Unbuffered, class Dispatcher> requires (N < sizeof...(Components))
        static void ViewTwo_(usize idx, entt::registry& registry, Dispatcher& dispatcher) {
            if (idx == N) {
                auto view = registry.view<
                    Get_t<(N - 1) % sizeof...(Components), Components...>,
                    Get_t<(N - 1) % sizeof...(Components), ComponentLists>...,
                    Get_t<N, Components...>,
                    Get_t<N, ComponentLists>...,
                    Unbuffered...>();
                dispatcher(view);
            }
            else {
                ViewTwo_<N + 1, Unbuffered...>(idx, registry, dispatcher);
            }
        }

        template<usize N, typename... Unbuffered, class Dispatcher> requires (N >= sizeof...(Components))
        static void ViewOne_(usize idx, entt::registry&, const Dispatcher&) {
            VANTA_UNREACHABLE("Buffer index out of bounds; iterator {}/{}!", N, idx);
        }

        template<usize N, typename... Unbuffered, class Dispatcher> requires (N >= sizeof...(Components))
        static void ViewTwo_(usize idx, entt::registry&, const Dispatcher&) {
            VANTA_UNREACHABLE("Buffer index out of bounds; iterator {}/{}!", N, idx);
        }
    };
}

#define VANTA_COMPONENT_BUFFER_nth(name, n) \
    struct CONCAT(name, CONCAT(_, n)) : name { using Base = name; };

#define VANTA_COMPONENT_BUFFER_2(name) \
    VANTA_COMPONENT_BUFFER_nth(name, 1)

#define VANTA_COMPONENT_BUFFER_3(name) \
    VANTA_COMPONENT_BUFFER_2(name)     \
    VANTA_COMPONENT_BUFFER_nth(name, 2)

#define VANTA_COMPONENT_BUFFER_4(name) \
    VANTA_COMPONENT_BUFFER_3(name)     \
    VANTA_COMPONENT_BUFFER_nth(name, 3)

#define VANTA_COMPONENT_BUFFFER_LIST_2(name) \
    using CONCAT(name, Buffers) = ComponentList<name, CONCAT(name, _1)>;

#define VANTA_COMPONENT_BUFFFER_LIST_3(name) \
    using CONCAT(name, Buffers) = ComponentList<name, CONCAT(name, _1), CONCAT(name, _2)>;

#define VANTA_COMPONENT_BUFFFER_LIST_4(name) \
    using CONCAT(name, Buffers) = ComponentList<name, CONCAT(name, _1), CONCAT(name, _2), CONCAT(name, _3)>;


#define VANTA_COMPONENT_BUFFER(name, count) \
    CONCAT(VANTA_COMPONENT_BUFFER, CONCAT(_, count))(name) \
    CONCAT(VANTA_COMPONENT_BUFFFER_LIST, CONCAT(_, count))(name)
