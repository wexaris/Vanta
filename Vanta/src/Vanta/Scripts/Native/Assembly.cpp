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

            SAVE_FUNCTION(RegisterEngineFunctions);
            
            SAVE_FUNCTION(GetClassList);
            SAVE_FUNCTION(GetClassFunctions);

            SAVE_FUNCTION(GetComponentList);
            SAVE_FUNCTION(GetComponentHash);

            SAVE_FUNCTION(Destroy);

#undef SAVE_FUNCTION
        }

        void ScriptAssembly::RegisterEngineFunctions(const EngineFunctions& functions) const {
            VANTA_CORE_ASSERT(RegisterEngineFunctions_Impl, "Calling unknown native script function!");
            RegisterEngineFunctions_Impl(functions);
        }

        std::pair<const char* const*, usize> ScriptAssembly::GetClassList() const {
            VANTA_CORE_ASSERT(GetClassList_Impl, "Calling unknown native script function!");
            usize count;
            const char* const* data = GetClassList_Impl(count);
            return { data, count };
        }

        ClassFunctions* ScriptAssembly::GetClassFunctions(const char* className) const {
            VANTA_CORE_ASSERT(GetClassFunctions_Impl, "Calling unknown native script function!");
            return GetClassFunctions_Impl(className);
        }

        std::pair<const char* const*, usize> ScriptAssembly::GetComponentList() const {
            VANTA_CORE_ASSERT(GetComponentList_Impl, "Calling unknown native script function!");
            usize count;
            const char* const* data = GetComponentList_Impl(count);
            return { data, count };
        }

        usize ScriptAssembly::GetComponentHash(const char* className) const {
            VANTA_CORE_ASSERT(GetComponentHash_Impl, "Calling unknown native script function!");
            return GetComponentHash_Impl(className);
        }
    }
}
