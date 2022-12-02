#pragma once

namespace Vanta {
    
    struct Log {
        static void Debug(const char* message) {
            Internal.Log_Debug(message);
        }

        static void Info(const char* message) {
            Internal.Log_Info(message);
        }

        static void Warn(const char* message) {
            Internal.Log_Warn(message);
        }

        static void Error(const char* message) {
            Internal.Log_Error(message);
        }
    };
}
