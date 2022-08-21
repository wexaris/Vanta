#pragma once
#include "Vanta/Input/KeyCodes.hpp"
#include "Vanta/Input/MouseCodes.hpp"
#include <variant>

namespace Vanta {
    struct InputKey {
        std::variant<KeyCode, MouseCode> Value;
        InputKey() = default;
        InputKey(KeyCode val) : Value(val) {}
        InputKey(MouseCode val) : Value(val) {}
    };

    class Input {
    public:
        static void PollInputs();

        static bool IsKeyPressed(KeyCode key);
        static uint GetKeyRepeatCount(KeyCode key) {
            auto item = s_KeyRepeatCount.find(key);
            return (item != s_KeyRepeatCount.end()) ? item->second : 0;
        }
        static uint IncrementKeyRepeatCount(KeyCode key) { return s_KeyRepeatCount[key]++; }
        static void ResetKeyRepeatCount(KeyCode key)     { s_KeyRepeatCount[key] = 0; }

        static bool IsMouseButtonPressed(MouseCode button);
        static std::pair<float, float> GetMousePos();
        static float GetMouseX();
        static float GetMouseY();

    private:
        static std::unordered_map<KeyCode, uint> s_KeyRepeatCount;
    };
}
