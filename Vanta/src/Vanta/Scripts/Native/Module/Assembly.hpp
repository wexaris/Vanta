#pragma once
#include <Vanta/Module/Interface.hpp>
#include "Vanta/Util/PlatformUtils.hpp"

namespace Vanta {
    namespace Native {

        class ScriptAssembly {
        public:
            ScriptAssembly(const Path& filepath);

            void RegisterEngineFunctions(const EngineFunctions& functions) const;

            std::pair<const char* const*, usize> GetClassList() const;
            ClassFunctions* GetClassFunctions(const char* className) const;
            std::pair<const ClassField*, usize> GetClassFieldList(const char* className) const;

            std::pair<const char* const*, usize> GetComponentList() const;
            usize GetComponentHash(const char* componentName) const;

            bool IsLoaded() const { return (bool)m_Library; }

        private:
            DynamicLibrary m_Library;

            RegisterEngineFunctions_Fn RegisterEngineFunctions_Impl = nullptr;
            GetClassList_Fn GetClassList_Impl = nullptr;
            GetClassFunctions_Fn GetClassFunctions_Impl = nullptr;
            GetClassFieldList_Fn GetClassFieldList_Impl = nullptr;
            GetComponentList_Fn GetComponentList_Impl = nullptr;
            GetComponentHash_Fn GetComponentHash_Impl = nullptr;
            Destroy_Fn Destroy_Impl = nullptr;
        };
    }
}
