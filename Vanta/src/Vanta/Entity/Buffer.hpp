#pragma once
#include <entt/entt.hpp>

#include <execution>

namespace Vanta {

    template<typename... T>
    using ComponentList = entt::type_list<T...>;

    template<usize N, typename... Types>
    struct Get {
        using type = typename std::tuple_element<N, std::tuple<Types...>>::type;
    };

    template<usize N, typename... Types>
    struct Get<N, ComponentList<Types...>> {
        using type = typename Get<N, Types...>::type;
    };

    /*template<typename Type>
    struct BufferInstanceHandler {
        void Create(entt::registry& registry, entt::entity entity) {
            if (!registry.all_of<Type>(entity))
                registry.emplace_or_replace<Type>(entity);
        }

        void Destroy(entt::registry& registry, entt::entity entity) {
            if (registry.all_of<Type>(entity))
                registry.remove<Type>(entity);
        }
    };*/

    template<typename... ComponentLists>
    class Buffer;

    template<typename... Types, typename... ComponentLists>
    class Buffer<ComponentList<Types...>, ComponentLists...> {
    public:
        Buffer() = default;

        static void InitBuffers(entt::registry& registry) {
            // TODO: Automatically set up all buffers when one is added
            //([&]() {
            //    registry.on_construct<Types>().template connect<&BufferInstanceHandler<Types>::Create>(BufferInstanceHandler<Types>());
            //    registry.on_destroy<Types>().template connect<&BufferInstanceHandler<Types>::Destroy>(BufferInstanceHandler<Types>());
            //}(), ...);
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
            m_BufferIdx = (m_BufferIdx + 1) % sizeof...(Types);
        }

    private:
        using FirstIdents = entt::identifier<Types...>;

        usize m_BufferIdx = 0; // Current buffer index

        /// <summary>
        /// Iterate through variadic params looking for the buffer that matches current index.
        /// </summary>
        /// <typeparam name="N">Current iteration.</typeparam>
        /// <typeparam name="Unbuffered">Other non-buffered types to retrieve.</typeparam>
        /// <param name="registry">Entity registry.</param>
        /// <param name="dispatcher">Dispatcher that will handle the view.</param>
        template<usize N, typename... Unbuffered, class Dispatcher> requires (N < sizeof...(Types))
        void View_(entt::registry& registry, const Dispatcher& dispatcher) {
            if (m_BufferIdx == N) {
                auto view = registry.view<
                    typename Get<N, Types...>::type,
                    typename Get<N, ComponentLists>::type..., Unbuffered...>();
                dispatcher(view);
            } else {
                View_<N + 1, Unbuffered...>(registry, dispatcher);
            }
        }

        template<usize N, typename... Unbuffered, class Dispatcher> requires (N == sizeof...(Types))
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

#define VANTA_COMPONENT_BUFFER_2(name) \
    struct CONCAT(name, _1) : name {}; \
    struct CONCAT(name, _2) : name {};

#define VANTA_COMPONENT_BUFFER_3(name) \
    VANTA_COMPONENT_BUFFER_2(name)     \
    struct CONCAT(name, _3) : name {};

#define VANTA_COMPONENT_BUFFER_4(name) \
    VANTA_COMPONENT_BUFFER_3(name)     \
    struct CONCAT(name, _4) : name {};

#define VANTA_COMPONENT_BUFFFER_LIST_0(name) \
    using CONCAT(name, List) = int;

#define VANTA_COMPONENT_BUFFFER_LIST_1(name) \
    using CONCAT(name, List) = ComponentList<CONCAT(name, _1)>;

#define VANTA_COMPONENT_BUFFFER_LIST_2(name) \
    using CONCAT(name, List) = ComponentList<CONCAT(name, _1), CONCAT(name, _2)>;

#define VANTA_COMPONENT_BUFFFER_LIST_3(name) \
    using CONCAT(name, List) = ComponentList<CONCAT(name, _1), CONCAT(name, _2), CONCAT(name, _3)>;

#define VANTA_COMPONENT_BUFFFER_LIST_4(name) \
    using CONCAT(name, List) = ComponentList<CONCAT(name, _1), CONCAT(name, _2), CONCAT(name, _3), CONCAT(name, _4)>;

#define VANTA_COMPONENT_BUFFER(name, count) \
    CONCAT(VANTA_COMPONENT_BUFFER, CONCAT(_, count))(name) \
    CONCAT(VANTA_COMPONENT_BUFFFER_LIST, CONCAT(_, count))(name)
