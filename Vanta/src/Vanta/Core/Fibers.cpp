#include "vantapch.hpp"
#include "Vanta/Core/Fibers.hpp"

namespace Vanta {

    uint Fibers::THREAD_COUNT = std::thread::hardware_concurrency();

    boost::thread_group Fibers::s_Workers;
    std::stack<Fibers::FiberList> Fibers::s_JobStack;

    static fibers::mutex s_StopMutex;
    static fibers::condition_variable s_StopCondition;

    static void WorkerRun() {
        // Continue to work while waiting on the stop condition
        std::unique_lock<fibers::mutex> lock(s_StopMutex);
        s_StopCondition.wait(lock);
    }

    void Fibers::Init() {
        VANTA_PROFILE_FUNCTION();

        // Spawn worker threads (start at 1 because the caller will be one of the workers)
        for (uint i = 1; i < THREAD_COUNT; i++) {
            s_Workers.create_thread([&]() {
                fibers::use_scheduling_algorithm<fibers::algo::work_stealing>(THREAD_COUNT, true);
                WorkerRun();
            });
        }
        
        // Set scheduling for the calling thread as well
        fibers::use_scheduling_algorithm<fibers::algo::work_stealing>(THREAD_COUNT, true);
    }

    void Fibers::Shutdown() {
        VANTA_PROFILE_FUNCTION();

        {
            // Notify worker threads about shutdown, and wait for them to exit
            std::unique_lock<fibers::mutex> lock(s_StopMutex);
            s_StopCondition.notify_all();
        }

        s_Workers.join_all();
    }

    void Fibers::Begin(usize jobCount) {
        VANTA_PROFILE_FUNCTION();
        auto fibers = FiberList();
        fibers.reserve(jobCount);
        s_JobStack.push(std::move(fibers));
    }

    void Fibers::End() {
        VANTA_PROFILE_FUNCTION();
        for (auto& fiber : s_JobStack.top())
            fiber.join();
        s_JobStack.pop();
    }
}
