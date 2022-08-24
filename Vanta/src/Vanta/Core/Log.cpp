#include "vantapch.hpp"
#include "Vanta/Core/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Vanta {
    extern void CreateLogSinks(Log::SinkList& sinks);

    Ref<spdlog::logger> Log::s_CoreLogger;
    Ref<spdlog::logger> Log::s_ClientLogger;

    void Log::Init(const Path& logfile) {
        SinkList sinks;

        // Create sinks
        auto logfile_sink = NewRef<spdlog::sinks::basic_file_sink_mt>(logfile.string(), true);
        logfile_sink->set_pattern("[%T] [%1] %n: %v");
        sinks.push_back(logfile_sink);

#ifndef VANTA_DISTRIB
        auto stdout_sink = NewRef<spdlog::sinks::stdout_color_sink_mt>();
        stdout_sink->set_pattern("[%T] [%1] %n: %v");
        sinks.push_back(stdout_sink);
#endif

        CreateLogSinks(sinks);

        // Create loggers
        s_CoreLogger = NewRef<spdlog::logger>("VANTA", sinks.begin(), sinks.end());
        spdlog::register_logger(s_CoreLogger);
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::trace);
        s_CoreLogger->set_pattern("%^[%T] %n: %v%$");

        s_ClientLogger = NewRef<spdlog::logger>("APP", sinks.begin(), sinks.end());
        spdlog::register_logger(s_ClientLogger);
        s_ClientLogger->set_level(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::trace);
        s_ClientLogger->set_pattern("%^[%T] %n: %v%$");
    }
}
