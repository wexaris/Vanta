#pragma once
#include <functional>
#include <string>
#include <unordered_map>

namespace Vanta {
    
    using UUID = uint64;

    class Entity;

    namespace Native {

        class Registry {
        public:
            static bool RegisterClass(
                const char* className,
                Entity* (*constructor)(UUID),
                void (*onCreate)(Entity*),
                void (*onUpdate)(Entity*, double),
                void (*onDestroy)(Entity*))
            {
                Registry& registry = Get();
                registry.m_ClassList.push_back(className);
                registry.m_ClassMap[className] = {
                    (ScriptObject* (*) (UUID))constructor,
                    (void (*) (ScriptObject*))onCreate,
                    (void (*) (ScriptObject*, double))onUpdate,
                    (void (*) (ScriptObject*))onDestroy
                };
                return true;
            }

            static bool RegisterComponent(const char* componentName, usize hash) {
                Registry& registry = Get();
                registry.m_ComponentList.push_back(componentName);
                registry.m_ComponentMap[componentName] = hash;
                return true;
            }

            static const std::vector<const char*>& GetClassList() {
                Registry& registry = Get();
                return registry.m_ClassList;
            }

            static ClassFunctions* GetClassFunctions(const char* className) {
                Registry& registry = Get();
                auto it = registry.m_ClassMap.find(className);
                if (it == registry.m_ClassMap.end())
                    return nullptr;
                return &it->second;
            }

            static const std::vector<const char*>& GetComponentList() {
                Registry& registry = Get();
                return registry.m_ComponentList;
            }

            static usize GetComponentHash(const char* componentName) {
                Registry& registry = Get();
                auto it = registry.m_ComponentMap.find(componentName);
                if (it == registry.m_ComponentMap.end())
                    return 0;
                return it->second;
            }

        private:
            std::vector<const char*> m_ClassList;
            std::unordered_map<std::string, ClassFunctions> m_ClassMap;

            std::vector<const char*> m_ComponentList;
            std::unordered_map<std::string, usize> m_ComponentMap;

            static Registry& Get() {
                static Registry instance;
                return instance;
            }
        };
    }
}
