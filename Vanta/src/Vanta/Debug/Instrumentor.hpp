#pragma once

namespace Vanta {
    /// <summary>
    /// The result of an instrumentation session.
    /// </summary>
    struct ProfileResult {
        using FloatMicroseconds = std::chrono::duration<double, std::micro>;

        const char* Name;
        FloatMicroseconds Start;
        std::chrono::microseconds Elapsed;
        std::thread::id ThreadID;
    };

    /// <summary>
    /// The result of an instrumentation session.
    /// </summary>
    class Instrumentor {
    public:
        void BeginSession(const std::string& name, const Path& path = "results.json");
        void EndSession();

        void WriteProfile(const ProfileResult& result);

        static Instrumentor& Get() {
            static Instrumentor instrumentor;
            return instrumentor;
        }

    private:
        Instrumentor() = default;
        ~Instrumentor();

        void NonLockingEndSession();

        void WriteHeader();
        void WriteFooter();

        std::string* m_Session = nullptr;
        std::ofstream m_Output;
        std::mutex m_Lock;
    };

    /// <summary>
    /// Timer for keeping track of function execution times.
    /// </summary>
    class InstrumentationTimer {
    public:
        using Clock = std::chrono::steady_clock;

        InstrumentationTimer(const char* name);
        ~InstrumentationTimer();

        void Stop();

    private:
        ProfileResult m_Result;
        std::chrono::time_point<Clock> m_StartTime;
        bool m_Stopped = false;
    };
}

#ifdef VANTA_ENABLE_PROFILE
    #define VANTA_PROFILE_BEGIN(name, path) (::Vanta::Instrumentor::Get().BeginSession(name, path))
    #define VANTA_PROFILE_END()             (::Vanta::Instrumentor::Get().EndSession())
    #define VANTA_PROFILE_SCOPE(name)       ::Vanta::InstrumentationTimer CONCAT(timer, VANTA_LINE)(name)
    #define VANTA_PROFILE_FUNCTION()        VANTA_PROFILE_SCOPE(VANTA_FUNCSIG)
#else
    #define VANTA_PROFILE_BEGIN(name, path)
    #define VANTA_PROFILE_END()
    #define VANTA_PROFILE_SCOPE(name)
    #define VANTA_PROFILE_FUNCTION()
#endif

#ifdef VANTA_ENABLE_PROFILE_RENDER
    #define VANTA_PROFILE_RENDER_BEGIN(name, path) VANTA_PROFILE_BEGIN(name, path)
    #define VANTA_PROFILE_RENDER_END()             VANTA_PROFILE_END()
    #define VANTA_PROFILE_RENDER_SCOPE(name)       VANTA_PROFILE_SCOPE(name)
    #define VANTA_PROFILE_RENDER_FUNCTION()        VANTA_PROFILE_FUNCTION()
#else
    #define VANTA_PROFILE_RENDER_BEGIN(name, path)
    #define VANTA_PROFILE_RENDER_END()
    #define VANTA_PROFILE_RENDER_SCOPE(name)
    #define VANTA_PROFILE_RENDER_FUNCTION()
#endif
