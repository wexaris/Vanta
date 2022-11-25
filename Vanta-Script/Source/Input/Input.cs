using System;

namespace Vanta {

    public static class Input {
        public static bool IsKeyDown(KeyCode key) {
            return Internal.Input_IsKeyDown(key);
        }

        public static bool IsMouseDown(MouseButton button) {
            return Internal.Input_IsMouseDown(button);
        }
    }
}