#include "vantapch.hpp"
#include "Vanta/Script/Class.hpp"
#include "Vanta/Script/ScriptEngine.hpp"

#include <mono/jit/jit.h>

namespace Vanta {

    ScriptClass::ScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className)
        : m_NamespaceName(namespaceName), m_ClassName(className)
    {
        VANTA_PROFILE_FUNCTION();
        m_Class = mono_class_from_name(image, namespaceName.c_str(), className.c_str());
        if (!m_Class)
            VANTA_CORE_CRITICAL("Failed to retrieve class from C# assembly: {}.{}", namespaceName, className);
    }

    MonoObject* ScriptClass::Instantiate() const {
        VANTA_PROFILE_FUNCTION();
        MonoObject* object = ScriptEngine::CreateObject(m_Class);
        mono_runtime_object_init(object);
        return object;
    }

    MonoMethod* ScriptClass::TryGetMethod(const std::string& name, int paramCount) const {
        return mono_class_get_method_from_name(m_Class, name.c_str(), paramCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params) const {
        VANTA_CORE_ASSERT(instance, "Invalid script class instance!");
        VANTA_CORE_ASSERT(method, "Invalid script class method!");
        return mono_runtime_invoke(method, instance, params, nullptr);
    }

    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
        : m_ScriptClass(scriptClass)
    {
        VANTA_PROFILE_FUNCTION();

        m_Instance = m_ScriptClass->Instantiate();

        m_Constructor = ScriptEngine::GetEntityClass().TryGetMethod(".ctor", 1);
        VANTA_CORE_ASSERT(m_Constructor, "Script class missing valid constructor!");

        m_OnCreateMethod = m_ScriptClass->TryGetMethod("OnCreate", 0);
        m_OnUpdateMethod = m_ScriptClass->TryGetMethod("OnUpdate", 1);
        m_OnDestroyMethod = m_ScriptClass->TryGetMethod("OnDestroy", 0);

        // Call constructor with entity ID
        UUID entityID = entity.GetUUID();
        void* param = &entityID;
        m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
    }

    void ScriptInstance::OnCreate() {
        if (m_OnCreateMethod) {
            m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
        }
    }

    void ScriptInstance::OnUpdate(float delta) {
        if (m_OnUpdateMethod) {
            void* param = &delta;
            m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
        }
    }

    void ScriptInstance::OnDestroy() {
        if (m_OnDestroyMethod) {
            m_ScriptClass->InvokeMethod(m_Instance, m_OnDestroyMethod);
        }
    }

    bool ScriptInstance::GetFieldValue_Impl(const std::string& name, void* buffer) {
        VANTA_PROFILE_FUNCTION();

        const auto& fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end()) {
            VANTA_CORE_ASSERT(false, "Script class field not found!");
            return false;
        }

        const ScriptField& field = it->second;
        mono_field_get_value(m_Instance, field.MonoField, buffer);
        return true;
    }

    bool ScriptInstance::SetFieldValue_Impl(const std::string& name, const void* value) {
        VANTA_PROFILE_FUNCTION();

        const auto& fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end()) {
            VANTA_CORE_ASSERT(false, "Script class field not found!");
            return false;
        }

        const ScriptField& field = it->second;
        mono_field_set_value(m_Instance, field.MonoField, const_cast<void*>(value));
        return true;
    }
}
