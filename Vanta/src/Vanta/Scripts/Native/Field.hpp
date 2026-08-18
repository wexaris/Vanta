#pragma once
#include "Vanta/Module/Interface.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/Field.hpp"

namespace Vanta {
    namespace Scripts {

        class NativeScriptField : public ScriptField {
        public:
            NativeScriptField(std::string name, ScriptFieldType type, void (*getValueFunc)(ScriptObject*, void*), void (*setValueFunc)(ScriptObject*, const void*));

            void GetValue(ScriptInstance* instance, void* buffer) const override;
            void SetValue(ScriptInstance* instance, const void* value) const override;

        private:
            void (*GetValueFunc)(ScriptObject*, void*) = nullptr;
            void (*SetValueFunc)(ScriptObject*, const void*) = nullptr;
        };
    }
}