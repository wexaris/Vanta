#include "vantapch.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Project/Serializer.hpp"

namespace Vanta {

    static const char* CSharp_Proj =
#include "Vanta/Scripts/CSharp/BuildTemplate/Scripts_CSharp.csproj"
        ;

    static const char* Native_CMake =
#include "Vanta/Scripts/Native/BuildTemplate/CMakeLists.txt"
        ;

    Ref<Project> Project::New(const Path& root_dir) {
        s_ActiveProject = NewRef<Project>();

        s_ActiveProject->m_RootDirectory = root_dir;

        // Create directory structure
        auto& config = s_ActiveProject->m_Config;
        std::filesystem::create_directories(GetCacheDirectory());
        std::filesystem::create_directories(GetAssetDirectory());
        std::filesystem::create_directories(GetAssetDirectory() / config.InitialScenePath.parent_path());
        std::filesystem::create_directories(GetScriptDirectory(Scripts::ScriptType::CSharp) / "Source");
        std::filesystem::create_directories(GetScriptDirectory(Scripts::ScriptType::Native) / "Source");

        // Create script build files
        IO::File csharp_csproj(GetScriptDirectory(Scripts::ScriptType::CSharp) / "Scripts_CSharp.csproj");
        csharp_csproj.Write(CSharp_Proj);

        IO::File native_cmake(GetScriptDirectory(Scripts::ScriptType::Native) / "CMakeLists.txt");
        native_cmake.Write(Native_CMake);

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
