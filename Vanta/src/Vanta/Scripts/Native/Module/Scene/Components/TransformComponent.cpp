#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Module/Scene/Components/TransformComponent.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {

    using namespace Native;

    namespace NativeImpl {

        const glm::vec3& TransformComponent_GetPosition(UUID entityID) {
            static_assert(sizeof(Vector3) == sizeof(glm::vec3));

            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(entityID);
            VANTA_ASSERT(entity, "Entity referenced in script doesn't exist!");

            return entity.GetComponent<TransformComponent>().Get().GetPosition();
        }

        void TransformComponent_SetPosition(UUID entityID, const glm::vec3& pos) {
            static_assert(sizeof(Vector3) == sizeof(glm::vec3));

            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Engine scene context not set!");
            Entity entity = scene->GetEntityByID(entityID);
            VANTA_CORE_ASSERT(scene, "Engine scene context not set!");

            entity.GetComponent<TransformComponent>().Set().SetPosition(pos);
        }
    }
}
