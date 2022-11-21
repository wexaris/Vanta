#include "vantapch.hpp"
#include "Vanta/Input/Input.hpp"
#include "Vanta/Script/Interface.hpp"
#include "Vanta/Script/ScriptEngine.hpp"

#include <mono/metadata/object.h>

namespace Vanta {

#define VANTA_ADD_INTERNAL_CALL(name) mono_add_internal_call("Vanta.Internal::" #name, name)

    static void Trace(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        VANTA_TRACE(str);
        mono_free(str);
    }

    static void Info(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        VANTA_INFO(str);
        mono_free(str);
    }

    static void Warn(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        VANTA_WARN(str);
        mono_free(str);
    }

    static void Error(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        VANTA_ERROR(str);
        mono_free(str);
    }

    static void Critical(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        VANTA_CRITICAL(str);
        mono_free(str);
    }

    static void Entity_GetPosition(UUID id, glm::vec3* pos) {
        Scene* scene = ScriptEngine::GetContext();
        VANTA_CORE_ASSERT(scene, "Script engine context missing!");

        Entity entity = scene->GetEntityByID(id);
        *pos = entity.GetComponent<TransformComponent>().Get().GetPosition();
    }

    static void Entity_SetPosition(UUID id, glm::vec3* pos) {
        Scene* scene = ScriptEngine::GetContext();
        VANTA_CORE_ASSERT(scene, "Script engine context missing!");

        Entity entity = scene->GetEntityByID(id);
        TransformComponent& tr = entity.GetComponent<TransformComponent>().Set();
        tr.SetPosition(*pos);
    }

    static bool Input_IsKeyDown(KeyCode key) {
        return Input::IsKeyPressed(key);
    }

    void Interface::RegisterFunctions() {
        VANTA_ADD_INTERNAL_CALL(Trace);
        VANTA_ADD_INTERNAL_CALL(Info);
        VANTA_ADD_INTERNAL_CALL(Warn);
        VANTA_ADD_INTERNAL_CALL(Error);
        VANTA_ADD_INTERNAL_CALL(Critical);

        VANTA_ADD_INTERNAL_CALL(Entity_GetPosition);
        VANTA_ADD_INTERNAL_CALL(Entity_SetPosition);

        VANTA_ADD_INTERNAL_CALL(Input_IsKeyDown);
    }
}
