#pragma once
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace Vanta {
    class Log {
    public:
        using SinkList = std::vector<spdlog::sink_ptr>;

        static void Init(const Path& logfile);

        static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_ClientLogger;

        Log() = delete;
    };
}

#define VANTA_CORE_DEBUG(...) (::Vanta::Log::GetCoreLogger()->debug(__VA_ARGS__))
#define VANTA_CORE_TRACE(...) (::Vanta::Log::GetCoreLogger()->trace(__VA_ARGS__))
#define VANTA_CORE_INFO(...)  (::Vanta::Log::GetCoreLogger()->info(__VA_ARGS__))
#define VANTA_CORE_WARN(...)  (::Vanta::Log::GetCoreLogger()->warn(__VA_ARGS__))
#define VANTA_CORE_ERROR(...) (::Vanta::Log::GetCoreLogger()->error(__VA_ARGS__))
#define VANTA_CORE_CRITICAL(...) (::Vanta::Log::GetCoreLogger()->critical(__VA_ARGS__))

#define VANTA_TRACE(...) (::Vanta::Log::GetClientLogger()->trace(__VA_ARGS__))
#define VANTA_INFO(...)  (::Vanta::Log::GetClientLogger()->info(__VA_ARGS__))
#define VANTA_WARN(...)  (::Vanta::Log::GetClientLogger()->warn(__VA_ARGS__))
#define VANTA_ERROR(...) (::Vanta::Log::GetClientLogger()->error(__VA_ARGS__))
#define VANTA_CRITICAL(...) (::Vanta::Log::GetClientLogger()->critical(__VA_ARGS__))
