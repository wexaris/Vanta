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

    namespace StaticUtil {
        /// <summary>
        /// Data owning string suitable for constexpr function return types.
        /// </summary>
        template<usize N>
        struct ResultString {
            char Data[N];
        };

        /// <summary>
        /// Removes certain parts of an input string at compile time.
        /// </summary>
        /// <param name="string">Input string</param>
        /// <param name="remove">Removable text</param>
        /// <returns></returns>
        template<usize N, usize K>
        constexpr ResultString<N> StringRemove(const char(&string)[N], const char(&remove)[K]) {
            ResultString<N> result = {};

            usize srcIndex = 0;
            usize dstIndex = 0;
            while (srcIndex < N) {
                // Check ahead for complete match
                usize matchIndex = 0;
                while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && string[srcIndex + matchIndex] == remove[matchIndex])
                    matchIndex++;

                // Skip text if matched
                if (matchIndex == K - 1)
                    srcIndex += matchIndex;

                // Copy relevant text to result
                result.Data[dstIndex++] = (string[srcIndex] == '"') ? '\'' : string[srcIndex];
                srcIndex++;
            }
            return result;
        }

        /// <summary>
        /// Cleans up a function signature, removing unnecessary information.
        /// </summary>
        template<usize N>
        constexpr ResultString<N> SignatureClean(const char(&signature)[N]) {
            return StringRemove(signature, "__cdecl ");
        }

        /// <summary>
        /// Get the name from a function signature.
        /// Removes parameters, template arguments, and other extra symbols.
        /// </summary>
        template<usize N>
        constexpr ResultString<N> SignatureName(const char(&signature)[N]) {
            auto clean_sig = SignatureClean(signature);
            auto clean = clean_sig.Data;

            ResultString<N> result = {};

            // Find beginning of parameter list
            usize endIndex = N - 1;
            for (usize parenLvl = 0; endIndex > 0; endIndex--) {
                if (clean[endIndex] == ')') {
                    parenLvl++;
                }
                else if (clean[endIndex] == '(') {
                    parenLvl--;

                    if (parenLvl == 0)
                        break;
                }
            }

            // Find start of function name
            usize startIndex = endIndex - 1;
            for (usize chevLvl = 0; startIndex > 0; startIndex--) {
                if (clean[startIndex] == '>') {
                    chevLvl++;
                }
                else if (clean[startIndex] == '<') {
                    chevLvl--;
                }
                // Break if char isn't an identifier or scope (::)
                else if (!(clean[startIndex] >= 'a' && clean[startIndex] <= 'z') &&
                         !(clean[startIndex] >= 'A' && clean[startIndex] <= 'Z') &&
                         !(clean[startIndex] >= '0' && clean[startIndex] <= '9') &&
                         clean[startIndex] != '_' && clean[startIndex] != ':')
                {
                    // Don't break if inside < >
                    if (chevLvl == 0) {
                        startIndex++;
                        break;
                    }
                }
            }

            // Copy relevant text to result
            for (usize dstIndex = 0; startIndex < endIndex; dstIndex++, startIndex++) {
                result.Data[dstIndex] = clean[startIndex];
            }

            return result;
        }

        /// <summary>
        /// Parses a filepath and returns only the filename.
        /// Mainly used in the `VANTA_FILENAME` macro.
        /// </summary>
        constexpr const char* FileName(const char* path) {
            const char* file = path;

            while (char c = *path++)
                if (c == '/' || c == '\\')
                    file = path;

            return file;
        }
    }
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
