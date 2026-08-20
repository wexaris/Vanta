#include "vantapch.hpp"
#include "Vanta/Scripts/Field.hpp"
#include "Vanta/Scripts/Instance.hpp"
#include "Vanta/Scripts/ScriptEngine.hpp"

namespace Vanta {
    namespace Scripts {

        Ref<ScriptInstance> ScriptEngine::Instantiate(const std::string& className, Entity entity) const {
            VANTA_PROFILE_FUNCTION();
            VANTA_CORE_ASSERT(EntityClassExists(className), "Invalid class!");
            VANTA_CORE_ASSERT(entity.IsValid(), "Invalid entity!");

            Ref<ScriptInstance> instance = NewRef<ScriptInstance>(GetEntityClass(className), entity);

            // Set variables modified in editor
            auto fieldInstances = GetFieldInstances(entity);
            if (fieldInstances) {
                for (auto& [name, field] : fieldInstances->get()) {
                    instance->WriteFieldValue(name, field->GetFieldData());
                }
            }

            return instance;
        }
    }
}
