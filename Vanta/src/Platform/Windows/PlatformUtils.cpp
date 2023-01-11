#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Util/PlatformUtils.hpp"

#include <commdlg.h>
#include <ShlObj_core.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Vanta {

    DynamicLibrary::DynamicLibrary(const Path& filepath) {
        std::string path = filepath.string();
        m_Library = LoadLibrary((LPCSTR)path.c_str());
    }

    DynamicLibrary::~DynamicLibrary() {
        FreeLibrary((HMODULE)m_Library);
    }

    void* DynamicLibrary::GetFunction_Impl(const char* name) {
        return GetProcAddress((HMODULE)m_Library, (LPCSTR)name);
    }

    Path Platform::OpenDirectoryDialog() {
        Path ret;

        // The BROWSEINFO struct tells the shell 
        // how it should display the dialog.
        BROWSEINFO bi = { 0 };
        bi.lpszTitle = "Select Folder";
        bi.hwndOwner = glfwGetWin32Window((GLFWwindow*)Engine::Get().GetWindow().GetNativeWindow());
        bi.ulFlags = BIF_USENEWUI;

        // must call this if using BIF_USENEWUI
        OleInitialize(NULL);

        // Show the dialog and get the itemIDList for the 
        // selected folder.
        LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

        if (pIDL) {
            char szBuf[_MAX_PATH] = { '\0' };
            if (SHGetPathFromIDList(pIDL, szBuf)) {
                ret = szBuf;
            }
            else {
                VANTA_CORE_ERROR("File browsing error; invalid browsing location!");
            }

            // free the item id list
            CoTaskMemFree((LPVOID)pIDL);
        }

        OleUninitialize();

        return ret;
    }

    Path Platform::OpenFileDialog(const char* filter) {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Engine::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE)
            return Path(ofn.lpstrFile);

        return Path();
    }

    Path Platform::SaveFileDialog(const char* filter) {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Engine::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // Sets the default extension by extracting it from the filter
        ofn.lpstrDefExt = strchr(filter, '\0') + 1;

        if (GetSaveFileNameA(&ofn) == TRUE)
            return Path(ofn.lpstrFile);

        return Path();
    }
}
