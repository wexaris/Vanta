#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Assembly.hpp"

namespace Vanta {
    namespace Native {

        ScriptAssembly::ScriptAssembly(const Path& filepath)
            : m_Library(filepath)
        {
            if (!m_Library) {
                VANTA_CORE_ERROR("Failed to load native script assembly: {}", filepath);
                return;
            }

#define SAVE_FUNCTION(name) \
    CONCAT(name, _Impl) = m_Library.GetFunction<CONCAT(name, _Fn)>(#name);

            SAVE_FUNCTION(GetClassList);
            SAVE_FUNCTION(GetClassFunctions);
            SAVE_FUNCTION(Destroy);

#undef SAVE_FUNCTION
        }

        std::pair<const char* const*, usize> ScriptAssembly::GetClassList() const {
            VANTA_CORE_ASSERT(GetClassList_Impl, "");
            usize count;
            const char* const* data = GetClassList_Impl(count);
            return { data, count };
        }

        ClassFunctions* ScriptAssembly::GetClassFunctions(const char* className) const {
            VANTA_CORE_ASSERT(GetClassFunctions_Impl, "");
            return GetClassFunctions_Impl(className);
        }
    }
}
