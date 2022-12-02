#pragma once

namespace Vanta {
    namespace Editor {

        struct Scripts {
            static void RebuildCSharp(bool clean = false);
            static void RebuildNative(bool clean = false);
        };
    }
}
