#include "ScriptBuildCoordinator.hpp"

#include <Vanta/Project/Project.hpp>

namespace Vanta {
    namespace Editor {

        void ScriptBuildCoordinator::SetBuildConfiguration(ScriptBuildConfiguration configuration) {
            m_BuildConfiguration = configuration;
        }

        ScriptBuildConfiguration ScriptBuildCoordinator::GetBuildConfiguration() const {
            return m_BuildConfiguration;
        }

        bool ScriptBuildCoordinator::RebuildWithBuilder(const ScriptBuilder& builder, const Path& projectDirectory, bool clean, const char* failurePrefix) {
            ScriptBuildOptions options;
            options.Clean = clean;
            options.Configuration = m_BuildConfiguration;

            if (!builder.Build(projectDirectory, options)) {
                VANTA_CORE_ERROR("{}", failurePrefix);
                return false;
            }

            return true;
        }

        bool ScriptBuildCoordinator::RebuildNative(bool clean) {
            return RebuildWithBuilder(m_NativeBuilder, Project::GetRootDirectory() / "Scripts" / "Native", clean, "Native script rebuild failed");
        }

        bool ScriptBuildCoordinator::RebuildCSharp(bool clean) {
            return RebuildWithBuilder(m_CSharpBuilder, Project::GetRootDirectory() / "Scripts" / "CSharp", clean, "C# script rebuild failed");
        }

    }
}