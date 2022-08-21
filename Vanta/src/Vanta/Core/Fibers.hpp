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

        template<typename Fn, typename... Args>
        static Fiber Spawn(Fn&& fn, Args&&... args) {
            return Fiber(fn, std::forward<Args>(args)...);
        }

        template<typename Fn, typename... Args>
        static Fiber Spawn(fibers::launch policy, Fn&& fn, Args&&... args) {
            return Fiber(policy, fn, std::forward<Args>(args)...);
        }

        template<typename Fn, typename... Args>
        static Fiber Spawn(fibers::fiber_properties* properties, Fn&& fn, Args&&... args) {
            return Fiber(properties, fn, std::forward<Args>(args)...);
        }

        template<typename Fn, typename... Args>
        static Fiber Spawn(fibers::launch policy, fibers::fiber_properties* properties, Fn&& fn, Args&&... args) {
            return Fiber(policy, properties, fn, std::forward<Args>(args)...);
        }

        template<typename Fn, typename... Args>
        static void SpawnDetached(Fn&& fn, Args&&... args) {
            Spawn(fn, std::forward<Args>(args)...).detach();
        }

        template<typename Fn, typename... Args>
        static void SpawnDetached(fibers::launch policy, Fn&& fn, Args&&... args) {
            Spawn(policy, fn, std::forward<Args>(args)...).detach();
        }

        template<typename Fn, typename... Args>
        static void SpawnDetached(fibers::fiber_properties* properties, Fn&& fn, Args&&... args) {
            Spawn(properties, fn, std::forward<Args>(args)...).detach();
        }

        template<typename Fn, typename... Args>
        static void SpawnDetached(fibers::launch policy, fibers::fiber_properties* properties, Fn&& fn, Args&&... args) {
            Spawn(policy, properties, fn, std::forward<Args>(args)...).detach();
        }

        static uint THREAD_COUNT;

    private:
        static boost::thread_group s_Workers;

        Fibers() = delete;
    };
}
