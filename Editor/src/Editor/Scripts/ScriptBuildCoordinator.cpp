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

        bool ScriptBuildCoordinator::RebuildWithBuilder(const ScriptBuilder& builder, bool clean) {
            if (!Project::GetActive()) {
                VANTA_CORE_ERROR("No active project to build scripts for");
                return false;
            }

            ScriptBuildOptions options;
            options.Clean = clean;
            options.Configuration = m_BuildConfiguration;
            return builder.Build(options);
        }

        bool ScriptBuildCoordinator::RebuildNative(bool clean) {
            return RebuildWithBuilder(m_NativeBuilder, clean);
        }

        bool ScriptBuildCoordinator::RebuildCSharp(bool clean) {
            return RebuildWithBuilder(m_CSharpBuilder, clean);
        }

    }
}