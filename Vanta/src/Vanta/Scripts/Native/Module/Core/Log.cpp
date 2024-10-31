#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Module/Core/Log.hpp"

namespace Vanta {
    namespace NativeImpl {

        void Log_Trace(const char* message) {
            VANTA_TRACE(message);
        }

        void Log_Info(const char* message) {
            VANTA_INFO(message);
        }

        void Log_Warn(const char* message) {
            VANTA_WARN(message);
        }

        void Log_Error(const char* message) {
            VANTA_ERROR(message);
        }
    }
}
