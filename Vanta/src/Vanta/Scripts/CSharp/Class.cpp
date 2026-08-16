#include "vantapch.hpp"
#include "Vanta/Scripts/CSharp/Class.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/class.h>

namespace Vanta {
    namespace CSharp {

        CSharpScriptClass::CSharpScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className, std::vector<Ref<ScriptField>> fields)
            : ScriptClass(std::move(fields)), m_NamespaceName(namespaceName), m_ClassName(className)
        {
            VANTA_PROFILE_FUNCTION();
            m_Class = mono_class_from_name(image, namespaceName.c_str(), className.c_str());
            if (!m_Class)
                VANTA_CORE_CRITICAL("Failed to retrieve class from C# assembly: {}.{}", namespaceName, className);

            m_Constructor = TryGetMethod(".ctor", 1);
            m_OnCreateMethod = TryGetMethod("OnCreate", 0);
            m_OnUpdateMethod = TryGetMethod("OnUpdate", 1);
            m_OnDestroyMethod = TryGetMethod("OnDestroy", 0);
        }

        void* CSharpScriptClass::InstantiateRuntimeInstance(Entity entity) const {
            VANTA_PROFILE_FUNCTION();
            VANTA_CORE_ASSERT(m_Class, "Invalid script class!");

            auto& entityBase = ScriptEngine::GetEntityClass();

            // Create new class instance
            MonoObject* object = ScriptEngine::CreateObject(m_Class);

            UUID entityID = entity.GetUUID();
            void* param = &entityID;
            InvokeMethod(object, entityBase.m_Constructor, &param);

            return object;
        }

        void CSharpScriptClass::InvokeOnCreate(const ScriptInstance* instance) const {
            VANTA_CORE_ASSERT(instance, "Invalid script class instance!");

            if (!m_OnCreateMethod)
                return;

            InvokeMethod((MonoObject*)instance->GetRuntimeInstance(), m_OnCreateMethod);
        }

        void CSharpScriptClass::InvokeOnUpdate(const ScriptInstance* instance, double delta) const {
            VANTA_CORE_ASSERT(instance, "Invalid script class instance!");

            if (!m_OnUpdateMethod)
                return;

            void* param = &delta;
            InvokeMethod((MonoObject*)instance->GetRuntimeInstance(), m_OnUpdateMethod, &param);
        }

        void CSharpScriptClass::InvokeOnDestroy(const ScriptInstance* instance) const {
            VANTA_CORE_ASSERT(instance, "Invalid script class instance!");

            if (!m_OnDestroyMethod)
                return;

            InvokeMethod((MonoObject*)instance->GetRuntimeInstance(), m_OnDestroyMethod);
        }

        MonoMethod* CSharpScriptClass::TryGetMethod(const std::string& name, int paramCount) const {
            VANTA_CORE_ASSERT(m_Class, "Invalid script class!");
            return mono_class_get_method_from_name(m_Class, name.c_str(), paramCount);
        }

        MonoObject* CSharpScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params) const {
            VANTA_CORE_ASSERT(instance, "Invalid script class instance!");
            VANTA_CORE_ASSERT(method, "Invalid script class method!");

            MonoObject* exception = nullptr;
            return mono_runtime_invoke(method, instance, params, &exception);
        }
    }
}
