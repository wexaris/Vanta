#include "vantapch.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Project/Serializer.hpp"

namespace Vanta {

    static const char* CSharp_CMake =
#include "Vanta/Scripts/CSharp/BuildTemplate/CMakeLists.txt"
        ;

    static const char* CSharp_Build =
#include "Vanta/Scripts/CSharp/BuildTemplate/Build.bat"
        ;

    static const char* Native_CMake =
#include "Vanta/Scripts/Native/BuildTemplate/CMakeLists.txt"
        ;

    static const char* Native_Build =
#include "Vanta/Scripts/Native/BuildTemplate/Build.bat"
        ;

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
        std::filesystem::create_directories(s_ActiveProject->m_RootDirectory / "Scripts" / "CSharp" / "Source");
        std::filesystem::create_directories(s_ActiveProject->m_RootDirectory / "Scripts" / "Native" / "Source");

        // Create script build files
        IO::File csharp_cmake(s_ActiveProject->m_RootDirectory / "Scripts" / "CSharp" / "CMakeLists.txt");
        IO::File csharp_build(s_ActiveProject->m_RootDirectory / "Scripts" / "CSharp" / "Build.bat");
        csharp_cmake.Write(CSharp_CMake);
        csharp_build.Write(CSharp_Build);

        IO::File native_cmake(s_ActiveProject->m_RootDirectory / "Scripts" / "Native" / "CMakeLists.txt");
        IO::File native_build(s_ActiveProject->m_RootDirectory / "Scripts" / "Native" / "Build.bat");
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
