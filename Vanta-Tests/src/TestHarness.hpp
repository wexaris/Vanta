#pragma once

#include <initializer_list>
#include <string>
#include <vector>

namespace Testing {

    struct Test {
        const char* Name;
        bool (*Function)();
    };

    class TestSet {
    public:
        TestSet(const char* name, std::initializer_list<Test> tests);
        bool IsGood() const;

    private:
        bool Run();
        bool RunOne(const Test& test);

        const char* m_Name;
        std::vector<Test> m_Tests;
        bool m_IsGood = true;
    };

}

#define TRUE_OR_FAIL(x) if (!(x)) { return false; } else {}
