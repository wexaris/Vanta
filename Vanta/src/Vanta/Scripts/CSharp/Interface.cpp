#include "vantapch.hpp"
#include "Vanta/Input/Input.hpp"
#include "Vanta/Scripts/CSharp/Interface.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <box2d/b2_body.h>

namespace Vanta {
    namespace CSharp {

        static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponent;

#define VANTA_ADD_INTERNAL_CALL(name) mono_add_internal_call("Vanta.Internal::" #name, name)

        static void Log_Trace(MonoString* msg) {
            char* str = mono_string_to_utf8(msg);
            VANTA_TRACE(str);
            mono_free(str);
        }

        static void Log_Info(MonoString* msg) {
            char* str = mono_string_to_utf8(msg);
            VANTA_INFO(str);
            mono_free(str);
        }

        static void Log_Warn(MonoString* msg) {
            char* str = mono_string_to_utf8(msg);
            VANTA_WARN(str);
            mono_free(str);
        }

        static void Log_Error(MonoString* msg) {
            char* str = mono_string_to_utf8(msg);
            VANTA_ERROR(str);
            mono_free(str);
        }

        static void Log_Critical(MonoString* msg) {
            char* str = mono_string_to_utf8(msg);
            VANTA_CRITICAL(str);
            mono_free(str);
        }

        static bool Input_IsKeyDown(KeyCode key) {
            return Input::IsKeyPressed(key);
        }

        static bool Input_IsMouseDown(MouseCode button) {
            return Input::IsMouseButtonPressed(button);
        }

        static uint64 Entity_GetEntityByName(MonoString* str) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");

            char* name = mono_string_to_utf8(str);
            Entity entity = scene->GetEntityByName(name);
            mono_free(name);

            if (!entity)
                return 0;

            return entity.GetUUID();
        }

        static MonoObject* Entity_GetScriptInstance(UUID id) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(id);
            VANTA_CORE_ASSERT(entity, "Entity referenced in script doesn't exist!");

            ScriptComponent* sc = entity.TryGetComponent<ScriptComponent>();
            if (!sc) {
                VANTA_ERROR("Entity doesn't have a script component!");
                return nullptr;
            }

            MonoObject* instance = sc->Instance->GetRuntimeInstance();
            VANTA_CORE_ASSERT(instance, "Script hasn't been instantiated!");

            return instance;
        }

        static bool Entity_HasComponent(UUID id, MonoReflectionType* type) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(id);
            VANTA_CORE_ASSERT(entity, "Entity referenced in script doesn't exist!");

            MonoType* componentType = mono_reflection_type_get_type(type);

            VANTA_CORE_ASSERT(s_EntityHasComponent.contains(componentType),
                "Component hasn't been registered with script engine!");

            return s_EntityHasComponent[componentType](entity);
        }

        static void TransformComponent_GetPosition(UUID id, glm::vec3* pos) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(id);
            VANTA_ASSERT(entity, "Entity referenced in script doesn't exist!");

            *pos = entity.GetComponent<TransformComponent>().Get().GetPosition();
        }

        static void TransformComponent_SetPosition(UUID id, glm::vec3* pos) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(id);
            VANTA_CORE_ASSERT(entity, "Entity referenced in script doesn't exist!");

            VANTA_CORE_ASSERT(entity.HasComponent<TransformComponent>(), "");
            TransformComponent& tr = entity.GetComponent<TransformComponent>().Set();
            tr.SetPosition(*pos);
        }

        static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID id, glm::vec2* impulse, bool wake) {
            Scene* scene = ScriptEngine::GetContext();
            VANTA_CORE_ASSERT(scene, "Script engine context not set!");
            Entity entity = scene->GetEntityByID(id);
            VANTA_ASSERT(entity, "Entity referenced in script doesn't exist!");

            Rigidbody2DComponent& rb = entity.GetComponent<Rigidbody2DComponent>();
            b2Body* body = (b2Body*)rb.RuntimeBody;
            body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
        }

        void Interface::RegisterFunctions() {
            VANTA_ADD_INTERNAL_CALL(Log_Trace);
            VANTA_ADD_INTERNAL_CALL(Log_Info);
            VANTA_ADD_INTERNAL_CALL(Log_Warn);
            VANTA_ADD_INTERNAL_CALL(Log_Error);
            VANTA_ADD_INTERNAL_CALL(Log_Critical);

            VANTA_ADD_INTERNAL_CALL(Input_IsKeyDown);
            VANTA_ADD_INTERNAL_CALL(Input_IsMouseDown);

            VANTA_ADD_INTERNAL_CALL(Entity_GetEntityByName);
            VANTA_ADD_INTERNAL_CALL(Entity_GetScriptInstance);
            VANTA_ADD_INTERNAL_CALL(Entity_HasComponent);

            VANTA_ADD_INTERNAL_CALL(TransformComponent_GetPosition);
            VANTA_ADD_INTERNAL_CALL(TransformComponent_SetPosition);

            VANTA_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
        }

        template<typename Component>
        static void RegisterComponent() {
            std::string_view name = typeid(Component).name();
            name = name.substr(name.rfind(':') + 1);

            std::string managedName = FMT("Vanta.{}", name);
            MonoImage* coreImage = ScriptEngine::GetCoreAssemblyImage();
            MonoType* type = mono_reflection_type_from_name(managedName.data(), coreImage);
            if (!type) {
                VANTA_CORE_ERROR("Script component type missing: {}", managedName);
                return;
            }

            s_EntityHasComponent[type] = [](Entity e) { return e.HasComponent<Component>(); };
        }

        template<typename... Components>
        static void RegisterComponent(ComponentList<Components...>) {
            ((RegisterComponent<Components>()), ...);
        }

        void Interface::RegisterComponents() {
            RegisterComponent(AllComponents());
        }
    }
}
