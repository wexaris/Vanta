#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Interface.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"
#include "Vanta/Scripts/Native/Module/Scene/Entity.hpp"

namespace Vanta {

    using namespace Native;

    namespace NativeImpl {

        // Map component ID to Entity::HasComponent
        static std::unordered_map<usize, std::function<bool(Entity)>> s_EntityHasComponentWithID;

#define VANTA_ENTITY_HAS_COMPONENT(name) \
    { #name, [](Entity entity) { return entity.HasComponent<name>(); }}

        // Map component name to Entity::HasComponent
        static std::unordered_map<std::string, std::function<bool(Entity)>> s_EntityHasComponentWithName = {
            VANTA_ENTITY_HAS_COMPONENT(TransformComponent),
            VANTA_ENTITY_HAS_COMPONENT(CameraComponent),
            VANTA_ENTITY_HAS_COMPONENT(Rigidbody2DComponent),
            VANTA_ENTITY_HAS_COMPONENT(BoxCollider2DComponent),
            VANTA_ENTITY_HAS_COMPONENT(CircleCollider2DComponent),
            VANTA_ENTITY_HAS_COMPONENT(SpriteComponent),
            VANTA_ENTITY_HAS_COMPONENT(CircleRendererComponent),
            VANTA_ENTITY_HAS_COMPONENT(ScriptComponent),
            VANTA_ENTITY_HAS_COMPONENT(NativeScriptComponent),
        };

        void RegisterComponent(const char* componentName, usize componentID) {
            // Since the native script API has its own component wrappers,
            // we map these component's IDs to interal component functions.
            s_EntityHasComponentWithID[componentID] = s_EntityHasComponentWithName[componentName];
        }

        uint64 Entity_GetEntityByName(const char* name) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");

            Entity entity = scene->GetEntityByName(name);
            if (!entity)
                return 0;

            return entity.GetUUID();
        }

        bool Entity_HasComponent(UUID entityID, usize componentID) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(entityID);
            VANTA_ASSERT(entity, "Entity referenced in script doesn't exist!");

            VANTA_CORE_ASSERT(s_EntityHasComponentWithID.contains(componentID),
                "Component hasn't been registered with script engine!");

            return s_EntityHasComponentWithID[componentID](entity);
        }
    }
}
