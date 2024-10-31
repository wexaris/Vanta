#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Module/Scene/Components/Rigidbody2DComponent.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"
#include <box2d/b2_body.h>

namespace Vanta {
    using namespace Native;

    namespace NativeImpl {

        void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, const glm::vec2& impulse, bool wake) {
            static_assert(sizeof(Vector2) == sizeof(glm::vec2));

            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(entityID);
            VANTA_ASSERT(entity, "Entity referenced in script doesn't exist!");

            Rigidbody2DComponent& rb = entity.GetComponent<Rigidbody2DComponent>();
            b2Body* body = (b2Body*)rb.RuntimeBody;
            body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), wake);
        }
    }
}
