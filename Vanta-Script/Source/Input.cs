using System;

namespace Vanta {

    public static class Input {
        public static bool IsKeyDown(KeyCode key) {
            return Internal.Input_IsKeyDown(key);
        }
    }
}