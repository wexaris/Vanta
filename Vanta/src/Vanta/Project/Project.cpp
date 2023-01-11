#include "vantapch.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Project/Serializer.hpp"

namespace Vanta {

    static const char* CSharp_CMake =
        "cmake_minimum_required(VERSION 3.20) \n"
        "project(Scripts_CSharp VERSION 0.1.0 LANGUAGES CSharp) \n"
        "set(VANTA_CORE_LIB \"\" CACHE STRING \"Path to Vanta-ScriptCore-CSharp.dll\") \n"
        "set(OUTPUT_DIR \"${CMAKE_BINARY_DIR}/../Binaries\") \n"
        "file(GLOB_RECURSE ${ PROJECT_NAME }_SOURCE \"Source/*.cs\") \n"
        "add_library(${ PROJECT_NAME } SHARED ${ ${PROJECT_NAME}_SOURCE }) \n"
        "set_property(TARGET ${ PROJECT_NAME } PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION \"v4.6.1\") \n"
        "set_target_properties(${ PROJECT_NAME } PROPERTIES \n"
        "    VS_DOTNET_REFERENCES \"${VS_DOTNET_REFERENCES}\" \n"
        "    VS_DOTNET_REFERENCE_Vanta - ScriptCore - CSharp \"${VANTA_CORE_LIB}\" \n"
        ") \n"
        "set_target_properties(${ PROJECT_NAME } PROPERTIES \n"
        "    RUNTIME_OUTPUT_DIRECTORY ${ OUTPUT_DIR } \n"
        "    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ OUTPUT_DIR } \n"
        "    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ OUTPUT_DIR } \n"
        "    ARCHIVE_OUTPUT_DIRECTORY ${ OUTPUT_DIR } \n"
        "    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${ OUTPUT_DIR } \n"
        "    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${ OUTPUT_DIR } \n"
        "    LIBRARY_OUTPUT_DIRECTORY ${ OUTPUT_DIR } \n"
        "    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${ OUTPUT_DIR } \n"
        "    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${ OUTPUT_DIR } \n"
        ") \n";

    static const char* CSharp_Build =
        "@echo off \n"
        "set \"CORE_LIB_DEBUG=E:/Code/engine/Vanta/build/x64-Debug-MSVC/lib/Vanta-ScriptCore-CSharp.dll\" \n" // TODO: Remove hard-coded path
        "set \"CORE_LIB_RELEASE=E:/Code/engine/Vanta/build/x64-Debug-MSVC/lib/Vanta-ScriptCore-CSharp.dll\" \n" // TODO: Remove hard-coded path
        "if \"%~1\" == \"\" ( \n"
        "    set \"BUILD_TYPE=Debug\" \n"
        ") else ( \n"
        "    set \"BUILD_TYPE=%1\" \n"
        ") \n"
        "if \"%BUILD_TYPE%\" == \"Release\" ( \n"
        "    set \"CORE_LIB=%CORE_LIB_RELEASE%\" \n"
        ") else ( \n"
        "    set \"CORE_LIB=%CORE_LIB_DEBUG%\" \n"
        ") \n"
        "cmake - S . - B Build - DVANTA_CORE_LIB:STRING = % CORE_LIB % \n"
        "cmake --build Build --config % BUILD_TYPE % \n"
        "pause \n";

    static const char* Native_CMake =
        "cmake_minimum_required(VERSION 3.20) \n"
        "project(Scripts_Native VERSION 0.1.0 LANGUAGES C CXX) \n"
        "set(CMAKE_CXX_STANDARD 20) \n"
        "set(VANTA_INCLUDE_DIR "" CACHE STRING \"Path to Vanta include directory\") \n"
        "set(OUTPUT_DIR \"${CMAKE_BINARY_DIR}/../Binaries\") \n"
        "file(GLOB_RECURSE ${ PROJECT_NAME }_SOURCE \"Source/*.cpp\") \n"
        "cmake_path(SET MODULE_SOURCE \"${VANTA_INCLUDE_DIR}/Vanta/Scripts/Native/Module/Vanta.cpp\") \n"
        "list(APPEND ${ PROJECT_NAME }_SOURCE ${ MODULE_SOURCE }) \n"
        "message(${ ${PROJECT_NAME}_SOURCE }) \n"
        "add_library(${ PROJECT_NAME } SHARED ${ ${PROJECT_NAME}_SOURCE }) \n"
        "target_include_directories(${ PROJECT_NAME } SYSTEM PRIVATE ${ VANTA_INCLUDE_DIR }) \n"
        "target_compile_definitions(${ PROJECT_NAME } PRIVATE - DVANTA_MODULE) \n"
        "set_target_properties(${ PROJECT_NAME } PROPERTIES \n"
        "    RUNTIME_OUTPUT_DIRECTORY ${ OUTPUT_DIR } \n"
        "    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ OUTPUT_DIR } \n"
        "    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ OUTPUT_DIR } \n"

        "    ARCHIVE_OUTPUT_DIRECTORY ${ OUTPUT_DIR } \n"
        "    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${ OUTPUT_DIR } \n"
        "    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${ OUTPUT_DIR } \n"

        "    LIBRARY_OUTPUT_DIRECTORY ${ OUTPUT_DIR } \n"
        "    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${ OUTPUT_DIR } \n"
        "    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${ OUTPUT_DIR } \n"
        ") \n";

    static const char* Native_Build =
        "@echo off \n"
        "set \"VANTA_INCLUDE_DIR=E:/Code/engine/Vanta/Vanta/src\" \n" // TODO: Remove hard-coded path
        "if \"%~1\" == \"\" ( \n"
        "    set \"BUILD_TYPE=Debug\" \n"
        ") else ( \n"
        "    set \"BUILD_TYPE=%1\" \n"
        ") \n"
        "cmake - S . - B Build - G \"Visual Studio 17 2022\" - DVANTA_INCLUDE_DIR:STRING = %VANTA_INCLUDE_DIR% \n"
        "cmake --build Build --config %BUILD_TYPE% \n"
        "pause \n";

    Ref<Project> Project::New(const Path& root_dir) {
        s_ActiveProject = NewRef<Project>();

        s_ActiveProject->m_RootDirectory = root_dir;

        // Create directory structure
        auto& config = s_ActiveProject->m_Config;
        std::filesystem::create_directories(GetCacheDirectory());
        std::filesystem::create_directories(GetAssetDirectory());
        std::filesystem::create_directories(GetAssetDirectory() / config.InitialScenePath.parent_path());
        std::filesystem::create_directories(s_ActiveProject->m_RootDirectory / config.CSharpScriptAssemblyPath.parent_path());
        std::filesystem::create_directories(s_ActiveProject->m_RootDirectory / config.NativeScriptAssemblyPath.parent_path());
        std::filesystem::create_directories(s_ActiveProject->m_RootDirectory / "Scripts_CSharp" / "Source");
        std::filesystem::create_directories(s_ActiveProject->m_RootDirectory / "Scripts_Native" / "Source");

        // Create script build files
        IO::File csharp_cmake(s_ActiveProject->m_RootDirectory / "Scripts_CSharp" / "CMakeLists.txt");
        IO::File csharp_build(s_ActiveProject->m_RootDirectory / "Scripts_CSharp" / "Build.bat");
        csharp_cmake.Write(CSharp_CMake);
        csharp_build.Write(CSharp_Build);

        IO::File native_cmake(s_ActiveProject->m_RootDirectory / "Scripts_Native" / "CMakeLists.txt");
        IO::File native_build(s_ActiveProject->m_RootDirectory / "Scripts_Native" / "Build.bat");
        native_cmake.Write(Native_CMake);
        native_build.Write(Native_Build);

        // Create default scene
        IO::File scene(GetAssetDirectory() / config.InitialScenePath);
        scene.Write("Scene:\n  Name: Untitled\n  Entities:\n");

        Save();
        return s_ActiveProject;
    }

    Ref<Project> Project::Load(const IO::File& file) {
        Ref<Project> project = NewRef<Project>();

        ProjectSerializer serializer(file);
        if (!serializer.Deserialize(project)) {
            VANTA_ERROR("Failed to parse project file: {}", file.Filepath);
            return nullptr;
        }

        s_ActiveProject = project;
        s_ActiveProject->m_RootDirectory = file.Filepath.parent_path();
        return s_ActiveProject;
    }

    void Project::Save() {
        Path filepath = s_ActiveProject->m_RootDirectory / (s_ActiveProject->m_Config.Name + ".vproj");
        ProjectSerializer serializer(filepath);
        serializer.Serialize(s_ActiveProject);
    }
}
