#pragma once
#include <boost/fiber/algo/round_robin.hpp>
#include <boost/fiber/algo/shared_work.hpp>
#include <boost/fiber/algo/work_stealing.hpp>
#include <boost/fiber/barrier.hpp>
#include <boost/fiber/condition_variable.hpp>
#include <boost/fiber/fiber.hpp>
#include <boost/thread/thread.hpp>

namespace fibers = boost::fibers;
namespace this_fiber = boost::this_fiber;

namespace Vanta {

    using Fiber = fibers::fiber;

    class Fibers {
    public:
        static void Init();
        static void Shutdown();

        static void Begin(usize jobCount);
        static void End();

        template<typename Fn, typename... Args>
        static void Spawn(Fn&& fn, Args&&... args) {
            auto fiber = Fiber(fn, std::forward<Args>(args)...);
            s_JobStack.top().push_back(std::move(fiber));
        }

        template<typename Fn, typename... Args>
        static void Spawn(fibers::launch policy, Fn&& fn, Args&&... args) {
            auto fiber = Fiber(policy, fn, std::forward<Args>(args)...);
            s_JobStack.top().push_back(std::move(fiber));
        }

        template<typename Fn, typename... Args>
        static void Spawn(fibers::fiber_properties* properties, Fn&& fn, Args&&... args) {
            auto fiber = Fiber(properties, fn, std::forward<Args>(args)...);
            s_JobStack.top().push_back(std::move(fiber));
        }

        template<typename Fn, typename... Args>
        static void Spawn(fibers::launch policy, fibers::fiber_properties* properties, Fn&& fn, Args&&... args) {
            auto fiber = Fiber(policy, properties, fn, std::forward<Args>(args)...);
            s_JobStack.top().push_back(std::move(fiber));
        }

        template<typename Fn, typename... Args>
        static void SpawnDetached(Fn&& fn, Args&&... args) {
            auto fiber = Fiber(fn, std::forward<Args>(args)...);
            fiber.detach();
        }

        template<typename Fn, typename... Args>
        static void SpawnDetached(fibers::launch policy, Fn&& fn, Args&&... args) {
            auto fiber = Fiber(policy, fn, std::forward<Args>(args)...);
            fiber.detach();
        }

        template<typename Fn, typename... Args>
        static void SpawnDetached(fibers::fiber_properties* properties, Fn&& fn, Args&&... args) {
            auto fiber = Fiber(properties, fn, std::forward<Args>(args)...);
            fiber.detach();
        }

        template<typename Fn, typename... Args>
        static void SpawnDetached(fibers::launch policy, fibers::fiber_properties* properties, Fn&& fn, Args&&... args) {
            auto fiber = Fiber(policy, properties, fn, std::forward<Args>(args)...);
            fiber.detach();
        }

        static uint THREAD_COUNT;

    private:
        using FiberList = std::vector<Fiber>;

        static boost::thread_group s_Workers;
        static std::stack<FiberList> s_JobStack;

        Fibers() = delete;
    };
}
