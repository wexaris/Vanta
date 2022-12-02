#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Class.hpp"

namespace Vanta {
    namespace Native {

        ScriptClass::ScriptClass(ScriptAssembly* assembly, const std::string& className)
            : m_ClassName(className)
        {
            VANTA_PROFILE_FUNCTION();
            m_Functions = *assembly->GetClassFunctions(className.c_str());
        }

        ScriptObject* ScriptClass::Instantiate(Entity entity) const {
            VANTA_PROFILE_FUNCTION();
            return m_Functions.Constructor(entity.GetUUID());
        }

        void ScriptClass::InvokeOnCreate(ScriptObject* instance) const {
            m_Functions.OnCreate(instance);
        }

        void ScriptClass::InvokeOnUpdate(ScriptObject* instance, double delta) const {
            m_Functions.OnUpdate(instance, delta);
        }

        void ScriptClass::InvokeOnDestroy(ScriptObject* instance) const {
            m_Functions.OnDestroy(instance);
        }

        ScriptInstance::ScriptInstance(Ref<ScriptClass> klass, Entity entity)
            : m_ScriptClass(klass)
        {
            VANTA_PROFILE_FUNCTION();
            m_Instance = m_ScriptClass->Instantiate(entity);
        }

        void ScriptInstance::OnCreate() {
            m_ScriptClass->InvokeOnCreate(m_Instance);
        }

        void ScriptInstance::OnUpdate(float delta) {
            m_ScriptClass->InvokeOnUpdate(m_Instance, delta);
        }

        void ScriptInstance::OnDestroy() {
            m_ScriptClass->InvokeOnDestroy(m_Instance);
        }
    }
}
