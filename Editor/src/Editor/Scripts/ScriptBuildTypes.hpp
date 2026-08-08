#pragma once

namespace Vanta {
    namespace Editor {

        enum class ScriptBuildConfiguration {
            Debug,
            Release,
            RelWithDebInfo,
            MinSizeRel,
        };

        struct ScriptBuildOptions {
            bool Clean = false;
            ScriptBuildConfiguration Configuration = ScriptBuildConfiguration::Debug;
        };

        inline const char* ToCMakeBuildType(ScriptBuildConfiguration configuration) {
            switch (configuration) {
            case ScriptBuildConfiguration::Debug:
                return "Debug";
            case ScriptBuildConfiguration::Release:
                return "Release";
            case ScriptBuildConfiguration::RelWithDebInfo:
                return "RelWithDebInfo";
            case ScriptBuildConfiguration::MinSizeRel:
                return "MinSizeRel";
            default:
                return "Debug";
            }
        }

        inline const char* ToDotnetConfiguration(ScriptBuildConfiguration configuration) {
            switch (configuration) {
            case ScriptBuildConfiguration::Debug:
                return "Debug";
            case ScriptBuildConfiguration::Release:
            case ScriptBuildConfiguration::RelWithDebInfo:
            case ScriptBuildConfiguration::MinSizeRel:
                return "Release";
            default:
                return "Debug";
            }
        }

    }
}