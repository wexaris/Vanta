#pragma once
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/Field.hpp"

extern "C" {
    typedef struct _MonoType MonoType;
    typedef struct _MonoClassField MonoClassField;
}

namespace Vanta {
    namespace CSharp {

        class CSharpScriptField : public ScriptField {
        public:
            CSharpScriptField(std::string name, MonoType* type, MonoClassField* field);
            CSharpScriptField(std::string name, ScriptFieldType type, MonoClassField* field);

            void GetValue(ScriptInstance* instance, void* buffer) const override;
            void SetValue(ScriptInstance* instance, const void* value) const override;

        private:
            MonoClassField* MonoField;
        };
    }
}
