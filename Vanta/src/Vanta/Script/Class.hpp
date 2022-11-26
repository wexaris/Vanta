#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Script/Field.hpp"

extern "C" {
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
}

namespace Vanta {

    class ScriptClass {
    public:
        ScriptClass() = default;
        ScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className);

        MonoObject* Instantiate() const;
        MonoMethod* TryGetMethod(const std::string& name, int paramCount) const;
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr) const;

        const std::unordered_map<std::string, ScriptField>& GetFields() const { return m_Fields; }

        operator MonoClass* () { return m_Class; }

    private:
        friend class ScriptEngine;

        MonoClass* m_Class = nullptr;

        std::unordered_map<std::string, ScriptField> m_Fields;

        std::string m_NamespaceName;
        std::string m_ClassName;
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

        MonoObject* m_Instance = nullptr;
        MonoMethod* m_Constructor = nullptr;
        MonoMethod* m_OnCreateMethod = nullptr;
        MonoMethod* m_OnUpdateMethod = nullptr;
        MonoMethod* m_OnDestroyMethod = nullptr;

        bool GetFieldValue_Impl(const std::string& name, void* buffer);
        bool SetFieldValue_Impl(const std::string& name, const void* data);
    };
}
