#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/CSharp/Field.hpp"

extern "C" {
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
}

namespace Vanta {
    namespace CSharp {

        class CSharpScriptClass : public ScriptClass {
        public:
            CSharpScriptClass() = default;
            CSharpScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className, std::vector<Ref<ScriptField>> fields);

            void InvokeOnCreate(const ScriptInstance* instance) const override;
            void InvokeOnUpdate(const ScriptInstance* instance, double delta) const override;
            void InvokeOnDestroy(const ScriptInstance* instance) const override;

            operator MonoClass* () { return m_Class; }

        private:
            friend class ScriptEngine;

            std::string m_ClassName;
            std::string m_NamespaceName;

            MonoClass* m_Class = nullptr;

            MonoMethod* m_Constructor = nullptr;
            MonoMethod* m_OnCreateMethod = nullptr;
            MonoMethod* m_OnUpdateMethod = nullptr;
            MonoMethod* m_OnDestroyMethod = nullptr;

            void* InstantiateRuntimeInstance(Entity entity) const override;

            MonoMethod* TryGetMethod(const std::string& name, int paramCount) const;
            MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr) const;
        };
    }
}
