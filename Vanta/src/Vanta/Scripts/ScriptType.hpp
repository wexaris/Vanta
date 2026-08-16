#pragma once

namespace Vanta {
    namespace Scripts {

        enum class ScriptType {
            CSharp,
            Native,
        };

        constexpr const char* ScriptTypeToString(ScriptType type) {
            switch (type) {
            case ScriptType::CSharp: return "CSharp";
            case ScriptType::Native: return "Native";
                // DO NOT add a default case!
                // Omitting it will cause a compiler warning if a new value is missing.
            }
            VANTA_CORE_ASSERT(false, "Unknown script type!");
            return "Unknown";
        }
    }
}
