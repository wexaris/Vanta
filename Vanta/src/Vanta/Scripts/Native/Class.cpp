#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Class.hpp"

namespace Vanta {
    namespace Native {

        NativeScriptClass::NativeScriptClass(ScriptAssembly* assembly, std::string className, std::vector<Ref<ScriptField>> fields)
            : ScriptClass(std::move(fields)), m_ClassName(std::move(className))
        {
            VANTA_PROFILE_FUNCTION();
            m_Functions = *assembly->GetClassFunctions(m_ClassName.c_str());
        }

        void* NativeScriptClass::InstantiateRuntimeInstance(Entity entity) const {
            VANTA_PROFILE_FUNCTION();
            return m_Functions.Constructor(entity.GetUUID());
        }

        void NativeScriptClass::InvokeOnCreate(const ScriptInstance* instance) const {
            m_Functions.OnCreate((ScriptObject*)instance->GetRuntimeInstance());
        }

        void NativeScriptClass::InvokeOnUpdate(const ScriptInstance* instance, double delta) const {
            m_Functions.OnUpdate((ScriptObject*)instance->GetRuntimeInstance(), delta);
        }

        void NativeScriptClass::InvokeOnDestroy(const ScriptInstance* instance) const {
            m_Functions.OnDestroy((ScriptObject*)instance->GetRuntimeInstance());
        }
    }
}
