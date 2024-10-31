#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Interface.hpp"
#include "Vanta/Scripts/Native/Module/Core/Log.hpp"
#include "Vanta/Scripts/Native/Module/Input/Input.hpp"
#include "Vanta/Scripts/Native/Module/Scene/Entity.hpp"
#include "Vanta/Scripts/Native/Module/Scene/Components/TransformComponent.hpp"
#include "Vanta/Scripts/Native/Module/Scene/Components/SpriteComponent.hpp"
#include "Vanta/Scripts/Native/Module/Scene/Components/Rigidbody2DComponent.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {

    using namespace NativeImpl;

    namespace Native {

        /// <summary>
        /// Force assign a function pointer to target.
        /// Lets us cast function parameters to easier-to-use types.
        /// </summary>
        template<typename Target, typename Source>
        static void ForceAssign(Target& target, Source&& source) { target = (Target)source; }
     
        void Interface::RegisterFunctions() {
            ScriptAssembly* assembly = ScriptEngine::GetAppAssembly();

            EngineFunctions functions;
#define VANTA_REGISTER_FUNCTION(name) ForceAssign(functions.name, name);

            VANTA_REGISTER_FUNCTION(Log_Trace);
            VANTA_REGISTER_FUNCTION(Log_Info);
            VANTA_REGISTER_FUNCTION(Log_Warn);
            VANTA_REGISTER_FUNCTION(Log_Error);

            VANTA_REGISTER_FUNCTION(Entity_GetEntityByName);
            VANTA_REGISTER_FUNCTION(Entity_HasComponent);

            VANTA_REGISTER_FUNCTION(Input_IsKeyDown);
            VANTA_REGISTER_FUNCTION(Input_IsMouseDown);

            VANTA_REGISTER_FUNCTION(TransformComponent_GetPosition);
            VANTA_REGISTER_FUNCTION(TransformComponent_SetPosition);

            VANTA_REGISTER_FUNCTION(SpriteComponent_SetColor);

            VANTA_REGISTER_FUNCTION(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

            assembly->RegisterEngineFunctions(functions);
        }

        void Interface::RegisterComponents() {
            ScriptAssembly* assembly = ScriptEngine::GetAppAssembly();

            auto [data, count] = assembly->GetComponentList();
            for (; count > 0; count--, data++) {
                const char* componentName = *data;
                usize componentID = assembly->GetComponentHash(componentName);
                RegisterComponent(componentName, componentID);
            }
        }
    }
}
