#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/Native/Field.hpp"
#include "Vanta/Scripts/Native/Module/Assembly.hpp"

namespace Vanta {
    namespace Scripts {

        class ScriptInstanceHandle;

        class NativeScriptClass : public ScriptClass {
        public:
            NativeScriptClass() = default;
            NativeScriptClass(ScriptAssembly* assembly, std::string className, std::vector<Ref<ScriptField>> fields);

            void InvokeOnCreate(const ScriptInstance* instance) const override;
            void InvokeOnUpdate(const ScriptInstance* instance, double delta) const override;
            void InvokeOnDestroy(const ScriptInstance* instance) const override;

        private:
            std::string m_ClassName;
            ClassFunctions m_Functions;

            Box<ScriptInstanceHandle> InstantiateRuntimeInstance(Entity entity) const override;
        };
    }
}
