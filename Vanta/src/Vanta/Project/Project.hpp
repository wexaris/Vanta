#pragma once

namespace Vanta {

    struct ProjectConfig {
        std::string Name = "Untitled";

        Path AssetDirectory = "Assets";
        Path CacheDirectory = "Cache";

        Path CSharpScriptAssemblyPath = "Scripts_CSharp/Binaries/Scripts_CSharp.dll";
        Path NativeScriptAssemblyPath = "Scripts_Native/Binaries/Scripts_Native.dll";

        Path InitialScenePath = "Scenes/Default.vnta";
    };

    class Project {
    public:
        static Ref<Project> New(const Path& root_dir);
        static Ref<Project> Load(const IO::File& file);
        static void Save();

        static const Path& GetRootDirectory() {
            VANTA_CORE_ASSERT(s_ActiveProject, "No project currently loaded!");
            return GetActive()->m_RootDirectory;
        }

        static Path GetAssetDirectory() {
            VANTA_CORE_ASSERT(s_ActiveProject, "No project currently loaded!");
            return GetRootDirectory() / GetActive()->m_Config.AssetDirectory;
        }

        static Path GetCacheDirectory() {
            VANTA_CORE_ASSERT(s_ActiveProject, "No project currently loaded!");
            return GetRootDirectory() / GetActive()->m_Config.CacheDirectory;
        }

        /// TODO: Move to an asset manager
        static Path GetAssetPath(const Path& filepath) {
            VANTA_CORE_ASSERT(s_ActiveProject, "No project currently loaded!");
            return GetAssetDirectory() / filepath;
        }

        static Path GetAssetPathRelative(const Path& filepath) {
            VANTA_CORE_ASSERT(s_ActiveProject, "No project currently loaded!");
            return std::filesystem::relative(filepath, GetAssetDirectory());
        }

        ProjectConfig& GetConfig() { return m_Config; }

        static Ref<Project> GetActive() {
            return s_ActiveProject;
        }

    private:
        Path m_RootDirectory;
        ProjectConfig m_Config;

        inline static Ref<Project> s_ActiveProject;
    };
}
