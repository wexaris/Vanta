#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Module/Scene/Components/SpriteComponent.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {
    namespace NativeImpl {

        void SpriteComponent_SetColor(UUID entityID, const glm::vec4& color) {
            static_assert(sizeof(Vector4) == sizeof(glm::vec4));

            Scripts::NativeScriptEngine& engine = Scripts::NativeScriptEngine::Get();

            Scene* scene = engine.GetContext();
            VANTA_CORE_ASSERT(scene, "Engine scene context not set!");
            Entity entity = scene->GetEntityByID(entityID);
            VANTA_CORE_ASSERT(scene, "Engine scene context not set!");

            entity.GetComponent<SpriteComponent>().Color = color;
        }
    }
}
