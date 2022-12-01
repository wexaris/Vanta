#pragma once

namespace Vanta {

    class DynamicLibrary {
    public:
        DynamicLibrary(const Path& filepath);
        ~DynamicLibrary();

        template<typename Func>
        Func GetFunction(const char* funcName) {
            return (Func)GetFunction_Impl(funcName);
        }

        operator bool() const {
            return m_Library;
        }

    private:
        void* m_Library;

        void* GetFunction_Impl(const char* name);
    };

    struct Platform {
        /// Open file selection dialog window.
        /// Returns empty if canceled.
        static Path OpenFileDialog(const char* filter);

        /// Open file storage dialog window.
        /// Returns empty if canceled.
        static Path SaveFileDialog(const char* filter);
    };
}
