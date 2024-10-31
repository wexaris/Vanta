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

        bool ScriptInstance::GetFieldValue_Impl(const std::string& name, void* buffer) {
            VANTA_PROFILE_FUNCTION();

            const auto& fields = m_ScriptClass->GetFields();
            auto it = fields.find(name);
            if (it == fields.end()) {
                VANTA_CORE_ASSERT(false, "Script class field not found!");
                return false;
            }

            const ScriptField& field = it->second;
            field.GetValue(m_Instance, buffer);
            return true;
        }

        bool ScriptInstance::SetFieldValue_Impl(const std::string& name, const void* value) {
            VANTA_PROFILE_FUNCTION();

            const auto& fields = m_ScriptClass->GetFields();
            auto it = fields.find(name);
            if (it == fields.end()) {
                VANTA_CORE_ASSERT(false, "Script class field not found!");
                return false;
            }

            const ScriptField& field = it->second;
            field.SetValue(m_Instance, value);
            return true;
        }
    }
}
