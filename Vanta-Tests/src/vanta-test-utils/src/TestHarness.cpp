#include <vanta-test-utils/TestHarness.hpp>
#include <Vanta/EntryPoint.hpp>

#include <iostream>

namespace Testing {

    TestSet::TestSet(const char* name, std::initializer_list<Test> tests)
        : m_Name(name), m_Tests(tests)
    {
        Run();
    }

    bool TestSet::IsGood() const {
        return m_IsGood;
    }

    bool TestSet::Run() {
        for (const auto& test : m_Tests) {
            try {
                m_IsGood = m_IsGood && RunOne(test);
            }
            catch (const std::exception& e) {
                std::cout << "Test `" << test.Name << "` threw an exception: " << e.what() << "\n";
                m_IsGood = false;
            }
            catch (...) {
                std::cout << "Test `" << test.Name << "` threw an unknown exception.\n";
                m_IsGood = false;
            }
        }

        if (m_IsGood)
            std::cout << "Test set `" << m_Name << "` succeeded!\n";
        else
            std::cout << "Test set `" << m_Name << "` failed!\n";

        return m_IsGood;
    }

    bool TestSet::RunOne(const Test& test) {
        bool good = test.Function();

        if (good)
            std::cout << "Test `" << test.Name << "` succeeded!\n";
        else
            std::cout << "Test `" << test.Name << "` failed!\n";

        return good;
    }

}
