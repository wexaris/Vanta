#include "vantapch.hpp"
#include "Vanta/Input/Input.hpp"
#include "Vanta/Scripts/Native/Interface.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {
    namespace Native {

        static std::unordered_map<usize, std::function<bool(Entity)>> s_EntityHasComponent;

        template<typename Target, typename Source>
        static void ForceAssign(Target& target, Source&& source) { target = (Target)source; }

#define VANTA_REGISTER_FUNCTION(name) ForceAssign(functions.name, name);

        static void Log_Debug(const char* message) {
            VANTA_DEBUG(message);
        }

        static void Log_Info(const char* message) {
            VANTA_INFO(message);
        }

        static void Log_Warn(const char* message) {
            VANTA_WARN(message);
        }

        static void Log_Error(const char* message) {
            VANTA_ERROR(message);
        }

        static bool Input_IsKeyDown(KeyCode key) {
            return Input::IsKeyPressed(key);
        }

        static bool Input_IsMouseDown(MouseCode button) {
            return Input::IsMouseButtonPressed(button);
        }

        static UUID Entity_GetEntityByName(const char* name) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");

            Entity entity = scene->GetEntityByName(name);
            if (!entity)
                return 0;

            return entity.GetUUID();
        }

        static bool Entity_HasComponent(UUID entityID, usize componentID) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(entityID);
            VANTA_ASSERT(entity, "Entity referenced in script doesn't exist!");

            VANTA_CORE_ASSERT(s_EntityHasComponent.contains(componentID),
                "Component hasn't been registered with script engine!");

            return s_EntityHasComponent[componentID](entity);
        }

        static const glm::vec3& TransformComponent_GetPosition(UUID entityID) {
            static_assert(sizeof(Vector3) == sizeof(glm::vec3));

            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(entityID);
            VANTA_ASSERT(entity, "Entity referenced in script doesn't exist!");

            return entity.GetComponent<TransformComponent>().Get().GetPosition();
        }

        static void TransformComponent_SetPosition(UUID entityID, const glm::vec3& pos) {
            static_assert(sizeof(Vector3) == sizeof(glm::vec3));

            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Engine scene context not set!");
            Entity entity = scene->GetEntityByID(entityID);
            VANTA_CORE_ASSERT(scene, "Engine scene context not set!");

            entity.GetComponent<TransformComponent>().Set().SetPosition(pos);
        }
     
        void Interface::RegisterFunctions() {
            ScriptAssembly* assembly = ScriptEngine::GetAppAssembly();

            EngineFunctions functions;
            VANTA_REGISTER_FUNCTION(Log_Debug);
            VANTA_REGISTER_FUNCTION(Log_Info);
            VANTA_REGISTER_FUNCTION(Log_Warn);
            VANTA_REGISTER_FUNCTION(Log_Error);

            VANTA_REGISTER_FUNCTION(Entity_GetEntityByName);
            VANTA_REGISTER_FUNCTION(Entity_HasComponent);

            VANTA_REGISTER_FUNCTION(Input_IsKeyDown);
            VANTA_REGISTER_FUNCTION(Input_IsMouseDown);

            VANTA_REGISTER_FUNCTION(TransformComponent_GetPosition);
            VANTA_REGISTER_FUNCTION(TransformComponent_SetPosition);

            assembly->RegisterEngineFunctions(functions);
        }

#define VANTA_ENTITY_HAS_COMPONENT(name) \
    { #name, [](Entity entity) { return entity.HasComponent<name>(); }}

        static std::unordered_map<std::string, std::function<bool(Entity)>> s_ComponentNameHasComponent = {
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

        void Interface::RegisterComponents() {
            ScriptAssembly* assembly = ScriptEngine::GetAppAssembly();

            auto [data, count] = assembly->GetComponentList();
            for (; count > 0; count--, data++) {
                const char* componentName = *data;
                usize componentHash = assembly->GetComponentHash(componentName);
                s_EntityHasComponent[componentHash] = s_ComponentNameHasComponent[componentName];
            }
        }
    }
}
