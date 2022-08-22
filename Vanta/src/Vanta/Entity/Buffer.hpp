#pragma once
#include <entt/entt.hpp>

#include <execution>

namespace Vanta {

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
            registry.on_construct<Component>().connect<ComponentList::AttachAll>();
            registry.on_destroy<Component>().connect<ComponentList::DetachAll>();
        }

        static void AttachAll(entt::registry& registry, entt::entity entity) {
            (Attach<Components>(registry, entity), ...);
        }

        static void DetachAll(entt::registry& registry, entt::entity entity) {
            (Detach<Components>(registry, entity), ...);
        }

        template<typename Component>
        static void Attach(entt::registry& registry, entt::entity entity) {
            auto c = Component();
            if (!registry.any_of<Component>(entity))
                registry.emplace<Component>(entity);
        }

        template<typename Component>
        static void Detach(entt::registry& registry, entt::entity entity) {
            auto c = Component();
            if (registry.any_of<Component>(entity))
                registry.remove<Component>(entity);
        }
    };

    template<usize N, typename... Types>
    struct Get {
        using type = typename std::tuple_element<N, std::tuple<Types...>>::type;
    };

    template<usize N, typename... Types>
    struct Get<N, ComponentList<Types...>> {
        using type = typename Get<N, Types...>::type;
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
        /// Run given function on current active data buffer.
        /// </summary>
        /// <typeparam name="Unbuffered">Other non-buffered types to retrieve.</typeparam>
        /// <param name="registry">Entity registry.</param>
        /// <param name="dispatcher">Dispatcher that will handle the view.</param>
        template<typename... Unbuffered, class Dispatcher>
        void View(entt::registry& registry, const Dispatcher& dispatcher) {
            View_<0, Unbuffered...>(registry, dispatcher);
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
        /// Iterate through variadic params looking for the buffer that matches current index.
        /// </summary>
        /// <typeparam name="N">Current iteration.</typeparam>
        /// <typeparam name="Unbuffered">Other non-buffered types to retrieve.</typeparam>
        /// <param name="registry">Entity registry.</param>
        /// <param name="dispatcher">Dispatcher that will handle the view.</param>
        template<usize N, typename... Unbuffered, class Dispatcher> requires (N < sizeof...(Components))
        void View_(entt::registry& registry, const Dispatcher& dispatcher) {
            if (m_BufferIdx == N) {
                auto view = registry.view<
                    typename Get<N, Components...>::type,
                    typename Get<N, ComponentLists>::type..., Unbuffered...>();
                dispatcher(view);
            } else {
                View_<N + 1, Unbuffered...>(registry, dispatcher);
            }
        }

        template<usize N, typename... Unbuffered, class Dispatcher> requires (N == sizeof...(Components))
        void View_(entt::registry&, const Dispatcher&) {
            VANTA_UNREACHABLE("Buffer index out of bounds; iterator {}/{}!", N, m_BufferIdx);
        }
    };

    /// <summary>
    /// Dispatcher that iterates over a collection of entities in parallel,
    /// on the engine's thread pool.
    /// </summary>
    /// <typeparam name="Functor">Functor to execute for every entity and its components.</typeparam>
    template<typename Functor>
    class ParalelDispatch {
    public:
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

            auto threads = Util::DistanceMin(beg, end, Fibers::THREAD_COUNT);

            if (threads > 1) {
                fibers::barrier bar(threads);
                for (; beg != end; ++beg) {
                    Fibers::SpawnDetached([=, &bar]() {
                        entt::entity entity = *beg;
                        auto components = view.template get<Components...>(entity);
                        auto args = std::tuple_cat(std::make_tuple(entity), components);
                        std::apply(func, args);
                        bar.wait();
                    });
                }
                bar.wait();
            }
            else {
                entt::entity entity = *beg;
                auto components = view.template get<Components...>(entity);
                auto args = std::tuple_cat(std::make_tuple(entity), components);
                std::apply(func, args);
            }
        }

    private:
        Functor m_Functor;

        ParalelDispatch(ParalelDispatch&&) = delete;
        ParalelDispatch(const ParalelDispatch&) = delete;
        ParalelDispatch operator=(const ParalelDispatch&) = delete;
    };

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
