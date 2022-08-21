#include "vantapch.hpp"
#include "Vanta/Debug/Instrumentor.hpp"

namespace fmt {
    template<>
    struct formatter<std::thread::id> : formatter<std::string> {
        template<typename Context>
        auto format(std::thread::id id, Context& ctx) const {
            return formatter<std::string>::format((std::stringstream() << id).str(), ctx);
        }
    };
}

namespace Vanta {
    Instrumentor::~Instrumentor() {
        if (m_Session) {
            VANTA_CORE_ERROR("Instrumetor session '{}' unterminated", *m_Session);
            EndSession();
        }
    }

    void Instrumentor::BeginSession(const std::string& name, const Path& path) {
        std::lock_guard<std::mutex> lock(m_Lock);

        if (m_Session) {
            if (Log::GetCoreLogger()) { // In case profiling before Log initialization
                VANTA_CORE_ERROR("Cannot begin Instrumetor session '{}' while session '{}' is in progress!", name, *m_Session);
            }
            NonLockingEndSession();
        }

        m_Output.open(path);
        if (m_Output.is_open()) {
            m_Session = new std::string(name);
            WriteHeader();
        }
        else if (Log::GetCoreLogger()) { // In case profiling before Log initialization
            VANTA_CORE_ERROR("Instrumentor failed to open results file: ", path.string());
        }
    }

    void Instrumentor::EndSession() {
        std::lock_guard<std::mutex> lock(m_Lock);
        NonLockingEndSession();
    }

    void Instrumentor::NonLockingEndSession() {
        if (m_Session) {
            WriteFooter();
            m_Output.close();
            delete m_Session;
            m_Session = nullptr;
        }
        else if (Log::GetCoreLogger()) {
            VANTA_CORE_ERROR("Failed to end session; profiling not in session!");
        }
    }

    void Instrumentor::WriteHeader() {
        m_Output << "{\"otherData\": {},\"traceEvents\":[{}";
        m_Output.flush();
    }

    void Instrumentor::WriteProfile(const ProfileResult& result) {
        std::string prof = FMT(
            ",{{"
            "\"cat\":\"function\","
            "\"dur\":{0},"
            "\"name\":\"{1}\","
            "\"ph\":\"X\","
            "\"pid\":0,"
            "\"tid\":{2},"
            "\"ts\":{3}"
            "}}",
            result.Elapsed.count(),
            result.Name,
            result.ThreadID,
            result.Start.count()
        );

        std::lock_guard<std::mutex> lock(m_Lock);
        if (m_Session) {
            m_Output << prof;
            m_Output.flush();
        }
        // NOTE: We're not throwing errors for non-session profiling because of static
        // lifetime objects being destroyed outside any instrumentation scope.
    }

    void Instrumentor::WriteFooter() {
        m_Output << "]}";
        m_Output.flush();
    }

    InstrumentationTimer::InstrumentationTimer(const char* name) :
        m_Result({ name, {}, {}, {} }),
        m_StartTime(Clock::now())
    {}

    InstrumentationTimer::~InstrumentationTimer() {
        if (!m_Stopped) {
            Stop();
        }
    }

    void InstrumentationTimer::Stop() {
        auto endTime = Clock::now();

        m_Result.Start = ProfileResult::FloatMicroseconds{ m_StartTime.time_since_epoch() };
        m_Result.Elapsed = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch() -
            std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch();
        m_Result.ThreadID = std::this_thread::get_id();

        Instrumentor::Get().WriteProfile(std::move(m_Result));
        m_Stopped = true;
    }
}
