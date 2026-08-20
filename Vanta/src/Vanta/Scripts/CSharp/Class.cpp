#include "vantapch.hpp"
#include "Vanta/Scripts/CSharp/Class.hpp"
#include "Vanta/Scripts/CSharp/Instance.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/class.h>

namespace Vanta {
    namespace Scripts {

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

        Box<ScriptInstanceHandle> CSharpScriptClass::InstantiateRuntimeInstance(Entity entity) const {
            VANTA_PROFILE_FUNCTION();
            VANTA_CORE_ASSERT(m_Class, "Invalid script class!");

            // Create new class instance
            Scripts::CSharpScriptEngine& engine = Scripts::CSharpScriptEngine::Get();

            uint32 handle = engine.CreateObject(m_Class);

            MonoObject* object = engine.GetObjectByHandle(handle);
            const Ref<CSharpScriptClass>& entityBase = engine.GetEntityClass();
            UUID entityID = entity.GetUUID();
            void* param = &entityID;
            InvokeMethod(object, entityBase->m_Constructor, &param);

            return NewBox<CSharpScriptInstanceHandle>(handle);
        }

        void CSharpScriptClass::InvokeOnCreate(const ScriptInstance* instance) const {
            VANTA_CORE_ASSERT(instance, "Invalid script class instance!");

            if (!m_OnCreateMethod)
                return;

            InvokeMethod((MonoObject*)instance->GetRuntimeObject(), m_OnCreateMethod);
        }

        void CSharpScriptClass::InvokeOnUpdate(const ScriptInstance* instance, double delta) const {
            VANTA_CORE_ASSERT(instance, "Invalid script class instance!");

            if (!m_OnUpdateMethod)
                return;

            void* param = &delta;
            InvokeMethod((MonoObject*)instance->GetRuntimeObject(), m_OnUpdateMethod, &param);
        }

        void CSharpScriptClass::InvokeOnDestroy(const ScriptInstance* instance) const {
            VANTA_CORE_ASSERT(instance, "Invalid script class instance!");

            if (!m_OnDestroyMethod)
                return;

            InvokeMethod((MonoObject*)instance->GetRuntimeObject(), m_OnDestroyMethod);
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
