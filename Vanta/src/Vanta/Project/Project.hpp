#pragma once
#include "Vanta/Scripts/ScriptType.hpp"

namespace Vanta {

    struct ProjectConfig {
        std::string Name = "Untitled";

        Path AssetDirectory = "Assets";
        Path CacheDirectory = "Cache";
        Path CSharpScriptDirectory = "Scripts/CSharp";
        Path NativeScriptDirectory = "Scripts/Native";

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

        static Path GetScriptDirectory(Scripts::ScriptType type) {
            VANTA_CORE_ASSERT(s_ActiveProject, "No project currently loaded!");
            switch (type) {
                case Scripts::ScriptType::CSharp:
                    return GetRootDirectory() / GetActive()->m_Config.CSharpScriptDirectory;
                case Scripts::ScriptType::Native:
                    return GetRootDirectory() / GetActive()->m_Config.NativeScriptDirectory;
                // DO NOT add a default case!
                // Omitting it will cause a compiler warning if a new value is missing.
            }
            VANTA_CORE_ASSERT(false, "Unknown script type!");
            return GetRootDirectory();
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
