#include <vantapch.hpp>
#include "Vanta/Scripts/Native/Module/Input/Input.hpp"
#include "Vanta/Input/Input.hpp"

namespace Vanta {
    namespace NativeImpl {

        bool Input_IsKeyDown(KeyCode key) {
            return Input::IsKeyPressed(key);
        }

        bool Input_IsMouseDown(MouseCode button) {
            return Input::IsMouseButtonPressed(button);
        }
    }
}
