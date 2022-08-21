#include "vantapch.hpp"
#include "Vanta/Core/Log.hpp"
#include "Vanta/GUI/ImGuiConsole.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Vanta {

    Ref<spdlog::logger> Log::s_CoreLogger;
    Ref<spdlog::logger> Log::s_ClientLogger;

    void Log::Init(const Path& path) {

        // Create sinks
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(NewRef<ConsoleSink_mt>()); // GUI console
        sinks.push_back(NewRef<spdlog::sinks::basic_file_sink_mt>(path.string(), true)); // logfile
        //sinks.push_back(NewRef<spdlog::sinks::stdout_color_sink_mt>()); // text console

        sinks[0]->set_pattern("%^[%T] %n: %v%$");
        sinks[1]->set_pattern("[%T] [%1] %n: %v");
        //sinks[2]->set_pattern("%^[%T] %n: %v%$");

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
