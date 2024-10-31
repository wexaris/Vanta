#pragma once

namespace Vanta {
    
    struct Log {
        static void Trace(const char* message) {
            Internal.Log_Trace(message);
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

#define VANTA_TRACE(...) (::Vanta::Log::Trace(VANTA_FMT(__VA_ARGS__)))
#define VANTA_INFO(...)  (::Vanta::Log::Trace(VANTA_FMT(__VA_ARGS__)))
#define VANTA_WARN(...)  (::Vanta::Log::Trace(VANTA_FMT(__VA_ARGS__)))
#define VANTA_ERROR(...) (::Vanta::Log::Trace(VANTA_FMT(__VA_ARGS__)))
