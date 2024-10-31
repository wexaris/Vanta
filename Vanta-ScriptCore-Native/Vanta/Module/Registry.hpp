#pragma once
#include <Vanta/Input/KeyCodes.hpp>
#include <Vanta/Input/MouseCodes.hpp>

#include <functional>
#include <string>
#include <unordered_map>

namespace Vanta {
    
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

                ClassFunctions functions = {
                    (ScriptObject* (*)(UUID))constructor,
                    (void (*)(ScriptObject*))onCreate,
                    (void (*)(ScriptObject*, double))onUpdate,
                    (void (*)(ScriptObject*))onDestroy
                };

                registry.m_ClassList.push_back(className);
                registry.m_ClassData[className] = { functions };
                return true;
            }

            static bool RegisterClassField(
                const char* className,
                const char* fieldName,
                const char* fieldType,
                void (*getter)(Entity*, void*),
                void (*setter)(Entity*, const void*))
            {
                Registry& registry = Get();
                
                auto it = registry.m_ClassData.find(className);
                if (it == registry.m_ClassData.end()) {
                    // TODO: Error?
                    return false;
                }

                ClassField field = {
                    fieldName, fieldType,
                    (void (*)(ScriptObject*, void*)) getter,
                    (void (*)(ScriptObject*, const void*)) setter
                };
                it->second.Fields.push_back(field);
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

            static const std::vector<ClassField>* GetClassFieldList(const char* className) {
                Registry& registry = Get();
                auto it = registry.m_ClassData.find(className);
                if (it == registry.m_ClassData.end())
                    return nullptr;
                return &it->second.Fields;
            }

            static ClassFunctions* GetClassFunctions(const char* className) {
                Registry& registry = Get();
                auto it = registry.m_ClassData.find(className);
                if (it == registry.m_ClassData.end())
                    return nullptr;
                return &it->second.Functions;
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
            struct ClassData {
                ClassFunctions Functions;
                std::vector<ClassField> Fields;
            };
            std::vector<const char*> m_ClassList;
            std::unordered_map<std::string, ClassData> m_ClassData;

            std::vector<const char*> m_ComponentList;
            std::unordered_map<std::string, usize> m_ComponentMap;

            static Registry& Get() {
                static Registry instance;
                return instance;
            }
        };
    }
}
