#pragma once
#include "Interop.hpp"

#include <functional>
#include <string>
#include <unordered_map>

namespace Vanta {
    
    using UUID = uint64;

    class EntityScript;

    namespace Native {

        class Registry {
        public:
            static bool RegisterClass(
                const char* className,
                EntityScript* (*constructor)(UUID),
                void (*onCreate)(EntityScript*),
                void (*onUpdate)(EntityScript*, double),
                void (*onDestroy)(EntityScript*))
            {
                Registry& registry = Get();
                registry.m_ClassList.push_back(className);
                registry.m_ClassMap[className] = { constructor, onCreate, onUpdate, onDestroy };
                return true;
            }

            static ClassFunctions* GetClassFunctions(const char* className) {
                Registry& registry = Get();
                auto it = registry.m_ClassMap.find(className);
                if (it == registry.m_ClassMap.end())
                    return nullptr;
                return &it->second;
            }

            static const std::vector<const char*>& GetClassList() {
                Registry& registry = Get();
                return registry.m_ClassList;
            }

        private:
            std::vector<const char*> m_ClassList;
            std::unordered_map<std::string, ClassFunctions> m_ClassMap;

            static Registry& Get() {
                static Registry instance;
                return instance;
            }
        };
    }
}
