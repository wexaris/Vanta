#include <vanta-test-utils/CoreTestsCommon.hpp>

namespace Testing {

    bool TestFibers() {
        Fibers::Init();

        Ref<std::atomic_int> number = NewRef<std::atomic_int>(1);

        ParallelBarrier barrier;
        barrier.StartFibers(50);

        for (int i = 1; i <= 50; i++) {
            Fibers::Spawn([](int id, ParallelBarrier* barrier, Ref<std::atomic_int> num) {
                for (int j = 1; j < id; j++) {
                    auto x = *num = (j % 2 == 0) ? (*num * j) : (*num + j);
                    this_fiber::yield();
                }
                barrier->WaitFiber();
            }, i, &barrier, number);
        }

        int linear = 1;
        for (int i = 1; i <= 50; i++) {
            for (int j = 1; j < i; j++) {
                linear = (j % 2 == 0) ? (linear * j) : (linear + j);
            }
        }

        barrier.Wait();

        TRUE_OR_FAIL(*number != linear);

        Fibers::Shutdown();
        return true;
    }
}
