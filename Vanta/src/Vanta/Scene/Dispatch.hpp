#pragma once
#include <entt/entt.hpp>

#include <execution>

namespace Vanta {

    /// <summary>
    /// Job dispatcher that iterates over a collection of entities and components in sequence,
    /// on the calling thread.
    /// </summary>
    template<typename... Components, typename Func>
    void LinearView(entt::registry& registry, Func&& func) {
        auto view = registry.view<Components...>();
        auto each = view.each();

        auto beg = each.begin();
        auto end = each.end();

        if (beg == end)
            return;

        for (; beg != end; ++beg) {
            std::apply(func, *beg);
        }
    };

    /// <summary>
    /// Synchronization object for coordinating fiber-based processing.
    /// To be used in conjunction with `ParallelView`, which handles work dispatch to fibers.
    /// </summary>
    class ParallelBarrier {
    public:
        void Wait() {
            if (m_Barrier != nullptr) {
                m_Barrier->wait();
                Fibers::End();
                delete m_Barrier;
                m_Barrier = nullptr;
            }
        }

    private:
        template<typename..., typename Func>
        friend void ParallelView(ParallelBarrier& barrier, entt::registry& registry, Func&& func);

        fibers::barrier* m_Barrier = nullptr;

        void StartFibers(usize jobCount) {
            Fibers::Begin(jobCount);
            m_Barrier = new fibers::barrier(jobCount + 1); // + 1 for this spawner thread
        }

        void WaitFiber() {
            if (m_Barrier != nullptr)
                m_Barrier->wait();
        }
    };

    /// <summary>
    /// Job dispatcher that iterates over a collection of entities in parallel,
    /// on the engine's thread pool.
    /// Makes use of `ParallelBarrier` for synchronizing with the caller thread.
    /// </summary>
    template<typename... Components, typename Func>
    void ParallelView(ParallelBarrier& barrier, entt::registry& registry, Func&& func) {
        static constexpr usize CHUNK_SIZE = 2;

        auto view = registry.view<Components...>();
        auto each = view.each();

        auto beg = each.begin();
        auto end = each.end();

        if (beg == end)
            return;

        usize jobCount = (usize)std::ceil((float)view.size_hint() / CHUNK_SIZE);

        if (jobCount > 1) {
            barrier.StartFibers(jobCount);

            for (usize i = 0; i < jobCount; i++) {
                Fibers::Spawn([&func](auto* barrier, auto beg, auto end) {
                    for (usize i = 0; i < CHUNK_SIZE && beg != end; ++i, ++beg) {
                        std::apply(func, *beg);
                    }
                    barrier->WaitFiber();
                }, &barrier, beg, end);

                for (usize j = 0; j < CHUNK_SIZE && beg != end; ++j)
                    ++beg;
            }
        }
        else {
            for (; beg != end; ++beg) {
                std::apply(func, *beg);
            }
        }
    };
}
