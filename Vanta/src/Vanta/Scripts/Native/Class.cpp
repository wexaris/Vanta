#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Class.hpp"
#include "Vanta/Scripts/Native/Instance.hpp"

namespace Vanta {
    namespace Scripts {

        NativeScriptClass::NativeScriptClass(ScriptAssembly* assembly, std::string className, std::vector<Ref<ScriptField>> fields)
            : ScriptClass(std::move(fields)), m_ClassName(std::move(className))
        {
            VANTA_PROFILE_FUNCTION();
            m_Functions = *assembly->GetClassFunctions(m_ClassName.c_str());
        }

        Box<ScriptInstanceHandle> NativeScriptClass::InstantiateRuntimeInstance(Entity entity) const {
            VANTA_PROFILE_FUNCTION();
            void* object = m_Functions.Constructor(entity.GetUUID());
            return NewBox<NativeScriptInstanceHandle>(object);
        }

        void NativeScriptClass::InvokeOnCreate(const ScriptInstance* instance) const {
            m_Functions.OnCreate((ScriptObject*)instance->GetRuntimeObject());
        }

        void NativeScriptClass::InvokeOnUpdate(const ScriptInstance* instance, double delta) const {
            m_Functions.OnUpdate((ScriptObject*)instance->GetRuntimeObject(), delta);
        }

        void NativeScriptClass::InvokeOnDestroy(const ScriptInstance* instance) const {
            m_Functions.OnDestroy((ScriptObject*)instance->GetRuntimeObject());
        }
    }
}
