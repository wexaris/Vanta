#pragma once
#include "Vanta/Scripts/Native/External/Interop.hpp"
#include "Vanta/Util/PlatformUtils.hpp"

namespace Vanta {
    namespace Native {

        class ScriptAssembly {
        public:
            ScriptAssembly(const Path& filepath);

            std::pair<const char* const*, usize> GetClassList() const;

            ClassFunctions* GetClassFunctions(const char* className) const;

            bool IsLoaded() const { return (bool)m_Library; }

        private:
            DynamicLibrary m_Library;

            GetClassList_Fn GetClassList_Impl = nullptr;
            GetClassFunctions_Fn GetClassFunctions_Impl = nullptr;
            Destroy_Fn Destroy_Impl = nullptr;
        };
    }
}
