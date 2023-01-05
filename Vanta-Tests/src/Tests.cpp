#include <Vanta/EntryPoint.hpp>
#include <Vanta/Vanta.hpp>

using namespace Vanta;

namespace Testing {

    /// ///////////////////////////////////////////////////
    /// SETUP

    struct Test {
        const char* Name;
        bool (*Function) ();
    };

    class TestSet {
    public:
        TestSet(const char* name, std::initializer_list<Test> tests)
            : m_Name(name), m_Tests(tests)
        {
            Run();
        }

        bool IsGood() const { return m_IsGood; }

    private:
        bool Run() {
            for (const auto& test : m_Tests) {
                m_IsGood = m_IsGood && RunOne(test);
            }

            if (m_IsGood)
                VANTA_CORE_INFO("Test set `{}` succeeded!", m_Name);
            else
                VANTA_CORE_ERROR("Test set `{}` failed!", m_Name);

            return m_IsGood;
        }

        bool RunOne(const Test& test) {
            bool good = test.Function();

            if (good)
                VANTA_CORE_INFO("Test `{}` succeeded!", test.Name);
            else
                VANTA_CORE_ERROR("Test `{}` failed!", test.Name);

            return good;
        }

        const char* m_Name;
        std::vector<Test> m_Tests;
        bool m_IsGood = true;
    };


    /// ///////////////////////////////////////////////////
    /// TESTS

#define TRUE_OR_FAIL(x) if (!(x)) { return false; } else {}

    bool TestEvents_MousePress(MouseButtonPressEvent&) { return true; };
    bool TestEvents_KeyPress(KeyPressEvent&) { return true; };

    bool TestMathDecompose() {
        glm::vec3 position_0 = { 1.0, 2.0, 3.0 };
        glm::vec3 rotation_0 = glm::radians(glm::vec3{ 4.0, 5.0, 6.0 });
        glm::vec3 scale_0 = { 7.0, 8.0, 9.0 };

        glm::mat4 transform =
            glm::translate(glm::mat4(1.f), position_0) *
            glm::mat4_cast(glm::quat(rotation_0)) *
            glm::scale(glm::mat4(1.f), scale_0);

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        Math::Decompose(transform, position, rotation, scale);

        float tolerance = 0.00001f; // ignore float inaccuracies
        {
            auto offX = abs(position.x - position_0.x);
            auto offY = abs(position.y - position_0.y);
            auto offZ = abs(position.z - position_0.z);
            TRUE_OR_FAIL(offX < tolerance&& offY < tolerance&& offZ < tolerance);
        }
        {
            auto offX = abs(rotation.x - rotation_0.x);
            auto offY = abs(rotation.y - rotation_0.y);
            auto offZ = abs(rotation.z - rotation_0.z);
            TRUE_OR_FAIL(offX < tolerance&& offY < tolerance&& offZ < tolerance);
        }
        {
            auto offX = abs(scale.x - scale_0.x);
            auto offY = abs(scale.y - scale_0.y);
            auto offZ = abs(scale.z - scale_0.z);
            TRUE_OR_FAIL(offX < tolerance&& offY < tolerance&& offZ < tolerance);
        }

        return true;
    }

    bool TestFibers() {
        Fibers::Init();

        Ref<std::atomic_int> number = NewRef<std::atomic_int>(1);

        ParallelBarrier barrier;
        barrier.StartFibers(50);

        for (int i = 1; i <= 50; i++) {
            Fibers::Spawn([](int id, ParallelBarrier* barrier, Ref<std::atomic_int> num) {
                for (int j = 1; j < id; j++) {
                    auto x = *num = (j % 2 == 0) ? (*num * j) : (*num + j);
                    //VANTA_CORE_INFO("[{}] {}", id, x);
                    this_fiber::yield();
                }
            barrier->WaitFiber();
            }, i, &barrier, number);
        }

        int linear = 1;
        for (int i = 1; i <= 50; i++) {
            for (int j = 1; j < i; j++) {
                linear = (j % 2 == 0) ? (linear * j) : (linear + j);
                //VANTA_CORE_INFO("{}", linear);
            }
        }

        barrier.Wait();

        TRUE_OR_FAIL(*number != linear);

        Fibers::Shutdown();
        return true;
    }

    bool TestEvents() {
        auto mousePress = MouseButtonPressEvent(Mouse::ButtonLeft);
        EventDispatcher dispatcher(mousePress);

        TRUE_OR_FAIL(dispatcher.Dispatch<MouseButtonPressEvent>(TestEvents_MousePress));
        TRUE_OR_FAIL(!dispatcher.Dispatch<KeyPressEvent>(TestEvents_KeyPress));

        return true;
    }
    
    bool TestBuffering() {
        Buffered<TransformComponent> tr_0;
        TRUE_OR_FAIL(&tr_0.Get() != &tr_0.Set());

        BufferedRegistry<TransformComponent> reg;
        auto entity = reg.Create();
        auto& tr_1 = reg.AddComponent<TransformComponent>(entity);

        auto scale333 = glm::vec3{ 3, 3, 3 };
        tr_1.Set().SetScale(scale333);

        reg.SwapBuffers();

        TRUE_OR_FAIL(tr_1.Get().GetScale() == scale333);
        TRUE_OR_FAIL(tr_1.Get().GetScale() != tr_1.Set().GetScale());

        reg.SwapBuffers();
        reg.SwapBuffersFwd();

        TRUE_OR_FAIL(tr_1.Get().GetScale() == tr_1.Set().GetScale());

        return true;
    }
}

using namespace Testing;

int main() {
    Log::Init("Vanta-Tests.log");

    TestSet testMath("Math", { { "MathDecompose", TestMathDecompose } });

    TestSet testFibers("Fibers", { {"Fibers", TestFibers } });

    TestSet testEvents("Events", { { "Events", TestEvents } });

    TestSet testComponents("Buffering", { { "Buffering", TestBuffering } });
}
