#pragma once
#include <Vanta/Input/KeyCodes.hpp>
#include <Vanta/Input/MouseCodes.hpp>

namespace Vanta {

    struct Input {
        static bool IsKeyDown(KeyCode key) {
            return Internal.Input_IsKeyDown((uint16)key);
        }

        static bool IsMouseDown(MouseCode button) {
            return Internal.Input_IsMouseDown((uint16)button);
        }
    };
}
