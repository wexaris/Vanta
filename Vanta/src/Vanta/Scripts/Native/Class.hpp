#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Native/Field.hpp"
#include "Vanta/Scripts/Native/Module/Assembly.hpp"

namespace Vanta {
    namespace Native {

        class ScriptInstance;

        class ScriptClass {
        public:
            ScriptClass() = default;
            ScriptClass(ScriptAssembly* assembly, const std::string& className);

            ScriptObject* Instantiate(Entity entity) const;
            void InvokeOnCreate(ScriptObject* instance) const;
            void InvokeOnUpdate(ScriptObject* instance, double delta) const;
            void InvokeOnDestroy(ScriptObject* instance) const;

            const std::unordered_map<std::string, ScriptField>& GetFields() const { return m_Fields; }

        private:
            friend class ScriptEngine;

            std::string m_ClassName;
            ClassFunctions m_Functions;
            std::unordered_map<std::string, ScriptField> m_Fields;
        };

        class ScriptInstance {
        public:
            ScriptInstance(Ref<ScriptClass> klass, Entity entity);

            void OnCreate();
            void OnUpdate(float delta);
            void OnDestroy();

            Ref<ScriptClass>& GetClass() { return m_ScriptClass; }

            template<typename T>
            T GetFieldValue(const std::string& name) {
                static char buffer[sizeof(T)];
                bool ok = GetFieldValue_Impl(name, buffer);
                if (!ok)
                    return T();
                return *(T*)buffer;
            }

            template<typename T>
            void SetFieldValue(const std::string& name, const T& value) {
                SetFieldValue_Impl(name, &value);
            }

        private:
            friend class ScriptEngine;

            Ref<ScriptClass> m_ScriptClass;

            ScriptObject* m_Instance = nullptr;

            bool GetFieldValue_Impl(const std::string& name, void* buffer);
            bool SetFieldValue_Impl(const std::string& name, const void* data);
        };
    }
}
