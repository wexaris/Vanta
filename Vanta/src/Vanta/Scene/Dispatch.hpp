#pragma once
#include "Vanta/Scene/BufferedRegistry.hpp"

#include <execution>

namespace Vanta {

    /// <summary>
    /// Job dispatcher that iterates over a collection of entities and components in sequence,
    /// on the calling thread.
    /// </summary>
    template<typename... Components, typename Registry, typename Func>
    void LinearView(Registry& registry, Func&& func) {
        registry.ViewIter<Components...>([&](auto, auto beg, auto end) {
            if (beg == end)
                return;

            for (; beg != end; ++beg) {
                std::apply(func, *beg);
            }
        });
    };

    /// <summary>
    /// Synchronization object for coordinating fiber-based processing.
    /// To be used in conjunction with `ParallelView`, which handles work dispatch to fibers.
    /// </summary>
    class ParallelBarrier {
    public:
        /// <summary>
        /// Setup function for synchronizer thread.
        /// </summary>
        /// <param name="jobCount">Number of jobs that will be executed</param>
        void StartFibers(usize jobCount) {
            Fibers::Begin(jobCount);
            m_Barrier = new fibers::barrier(jobCount + 1); // + 1 for this thread
        }

        /// <summary>
        /// Wait function for synchronizer thread.
        /// Waits for all running jobs to be complete.
        /// </summary>
        void Wait() {
            if (m_Barrier != nullptr) {
                m_Barrier->wait();
                Fibers::End();
                delete m_Barrier;
                m_Barrier = nullptr;
            }
        }

        /// <summary>
        /// Wait function for a fiber instance.
        /// Waits for all running jobs to be complete.
        /// </summary>
        void WaitFiber() {
            if (m_Barrier != nullptr)
                m_Barrier->wait();
        }

    private:
        fibers::barrier* m_Barrier = nullptr;
    };

    /// <summary>
    /// Job dispatcher that iterates over a collection of entities in parallel,
    /// on the engine's fiber pool.
    /// Makes use of `ParallelBarrier` for synchronizing with the caller thread.
    /// </summary>
    template<typename... Components, typename Registry, typename Func> requires (sizeof...(Components) > 1) // View has `size_hint()` for > 1 component
    void ParallelView(ParallelBarrier& barrier, Registry& registry, Func&& func) {
        // Number of entities each fiber will process
        static constexpr usize CHUNK_SIZE = 16;

        registry.ViewIter<Components...>([&](auto view, auto beg, auto end) {
            if (beg == end)
                return;

            // Divide processable entity count by chunk size
            usize jobCount = (usize)std::ceil((float)view.size_hint() / CHUNK_SIZE);

            if (jobCount > 1) {
                barrier.StartFibers(jobCount);

                for (usize i = 0; i < jobCount; i++) {
                    // Spawn a fiber to work on every CHUNK_SIZE of entities
                    Fibers::Spawn([&func](auto* barrier, auto beg, auto end) {
                        for (usize i = 0; i < CHUNK_SIZE && beg != end; ++i, ++beg) {
                            std::apply(func, *beg);
                        }
                        barrier->WaitFiber();
                    }, &barrier, beg, end);

                    // Move entity iterator forward by CHUNK_SIZE
                    for (usize j = 0; j < CHUNK_SIZE && beg != end; ++j)
                        ++beg;
                }
            }
            else {
                // If there's only a single chunk of entities,
                // process them right now
                for (; beg != end; ++beg) {
                    std::apply(func, *beg);
                }
            }
        });
    };

    template<typename... Components, typename Registry, typename Func> requires (sizeof...(Components) == 1) // View has `size()` for 1 component
    void ParallelView(ParallelBarrier& barrier, Registry& registry, Func&& func) {
        static constexpr usize CHUNK_SIZE = 16;

        registry.ViewIter<Components...>([&](auto view, auto beg, auto end) {
            if (beg == end)
                return;

            usize jobCount = (usize)std::ceil((float)view.size() / CHUNK_SIZE);

            if (jobCount > 1) {
                barrier.StartFibers(jobCount);

                for (usize i = 0; i < jobCount; i++) {
                    Fibers::Spawn([func](auto* barrier, auto beg, auto end) {
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
        });
    };
}
