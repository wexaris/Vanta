#pragma once
#include <Vanta/Input/KeyCodes.hpp>
#include <Vanta/Input/MouseCodes.hpp>

namespace Vanta {

    struct Input {
        static bool IsKeyDown(KeyCode key) {
            return Internal.Input_IsKeyDown(key);
        }

        static bool IsMouseDown(MouseCode button) {
            return Internal.Input_IsMouseDown(button);
        }
    };
}
