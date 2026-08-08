#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Util/DynamicLibrary.hpp"

namespace Vanta {

    DynamicLibrary::DynamicLibrary(const Path& filepath) {
        m_Library = LoadLibrary((LPCSTR)filepath.c_str());
    }

    DynamicLibrary::~DynamicLibrary() {
        FreeLibrary((HMODULE)m_Library);
    }

    void* DynamicLibrary::GetFunction_Impl(const char* name) {
        return GetProcAddress((HMODULE)m_Library, (LPCSTR)name);
    }
}
