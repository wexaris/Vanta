#include "vantapch.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Project/Serializer.hpp"

namespace Vanta {

    Ref<Project> Project::New(const IO::File& file) {
        s_ActiveProject = NewRef<Project>();

        s_ActiveProject->m_RootDirectory = file.Filepath.parent_path();

        auto& config = s_ActiveProject->m_Config;
        std::filesystem::create_directories(GetCacheDirectory());
        std::filesystem::create_directories(GetAssetDirectory());
        std::filesystem::create_directories(s_ActiveProject->m_RootDirectory / config.ScriptAssemblyPath.parent_path());
        std::filesystem::create_directories(s_ActiveProject->m_RootDirectory / config.InitialScenePath.parent_path());

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
