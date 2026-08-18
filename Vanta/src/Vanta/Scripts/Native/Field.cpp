#include "vantapch.hpp"
#include "Vanta/Scripts/Instance.hpp"
#include "Vanta/Scripts/Native/Field.hpp"

namespace Vanta {
    namespace Scripts {

        NativeScriptField::NativeScriptField(std::string name, ScriptFieldType type, void (*getValueFunc)(ScriptObject*, void*), void (*setValueFunc)(ScriptObject*, const void*))
            : ScriptField(std::move(name), type), GetValueFunc(getValueFunc), SetValueFunc(setValueFunc)
        {
        }

        void NativeScriptField::GetValue(ScriptInstance* instance, void* buffer) const {
            VANTA_CORE_ASSERT(instance, "Script object instance cannot be null!");
            VANTA_CORE_ASSERT(buffer, "Buffer cannot be null!");
            VANTA_CORE_ASSERT(GetValueFunc, "GetValueFunc cannot be null!");
            GetValueFunc((ScriptObject*)instance->GetRuntimeInstance(), buffer);
        }

        void NativeScriptField::SetValue(ScriptInstance* instance, const void* value) const {
            VANTA_CORE_ASSERT(instance, "Script object instance cannot be null!");
            VANTA_CORE_ASSERT(value, "Value cannot be null!");
            VANTA_CORE_ASSERT(SetValueFunc, "SetValueFunc cannot be null!");
            SetValueFunc((ScriptObject*)instance->GetRuntimeInstance(), value);
        }
    }
}
