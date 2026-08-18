#include "vantapch.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/Instance.hpp"
#include "Vanta/Scripts/Field.hpp"

namespace Vanta {
    namespace Scripts {

        ScriptInstance::ScriptInstance(Ref<ScriptClass> klass, Entity entity)
            : m_ScriptClass(std::move(klass))
        {
            VANTA_PROFILE_FUNCTION();
            m_RuntimeInstance = m_ScriptClass->InstantiateRuntimeInstance(entity);
        }

        void ScriptInstance::OnCreate() const {
            VANTA_PROFILE_FUNCTION();
            m_ScriptClass->InvokeOnCreate(this);
        }

        void ScriptInstance::OnUpdate(float delta) const {
            VANTA_PROFILE_FUNCTION();
            m_ScriptClass->InvokeOnUpdate(this, delta);
        }

        void ScriptInstance::OnDestroy() const {
            VANTA_PROFILE_FUNCTION();
            m_ScriptClass->InvokeOnDestroy(this);
        }

        bool ScriptInstance::ReadFieldValue(const std::string& name, void* buffer) {
            VANTA_PROFILE_FUNCTION();

            const auto& fields = m_ScriptClass->GetFields();
            auto it = fields.find(name);
            if (it == fields.end()) {
                VANTA_CORE_ASSERT(false, "Script class field not found!");
                return false;
            }

            const auto& field = it->second;
            field->GetValue(this, buffer);
            return true;
        }

        bool ScriptInstance::WriteFieldValue(const std::string& name, const void* value) {
            VANTA_PROFILE_FUNCTION();

            const auto& fields = m_ScriptClass->GetFields();
            auto it = fields.find(name);
            if (it == fields.end()) {
                VANTA_CORE_ASSERT(false, "Script class field not found!");
                return false;
            }

            const auto& field = it->second;
            field->SetValue(this, value);
            return true;
        }
    }
}
