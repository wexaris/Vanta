#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Interface.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {
    namespace Native {

#define VANTA_REGISTER_FUNCTION(name) functions.name = name;

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
     
        void Interface::RegisterFunctions() {
            ScriptAssembly* assembly = ScriptEngine::GetAppAssembly();

            EngineFunctions functions;
            VANTA_REGISTER_FUNCTION(Log_Debug);
            VANTA_REGISTER_FUNCTION(Log_Info);
            VANTA_REGISTER_FUNCTION(Log_Warn);
            VANTA_REGISTER_FUNCTION(Log_Error);

            assembly->RegisterEngineFunctions(functions);
        }
    }
}
