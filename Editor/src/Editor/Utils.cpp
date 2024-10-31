#include "Utils.hpp"
#include <Vanta/Vanta.hpp>
#include <Vanta/Project/Project.hpp>
#include <Vanta/Scripts/ScriptEngine.hpp>

namespace Vanta {
    namespace Editor {

        static constexpr const char* GetBuildType() {
#if defined(VANTA_RELEASE)
            return "Release";
#else
            return "Debug";
#endif
        }

        static bool InvokeBuild(const Path& directory, bool clean) {
            const char* buildType = GetBuildType();

            if (clean) {
                Path buildDir = directory / "Build";
                std::filesystem::remove_all(buildDir);
            }

#if defined(VANTA_PLATFORM_WINDOWS)
            Path path = directory / "Build.bat";
            std::string command = FMT("cd {} && {} {}", directory.string(), path.string(), buildType);
            int ret = system(command.c_str());
            return ret == 0;
#else
            VANTA_CORE_ERROR("Script rebuild not supported on this system!");
            return false;
#endif
        }

        void Scripts::RebuildCSharp(bool clean) {
            InvokeBuild(Project::GetRootDirectory() / "Scripts" / "CSharp", clean);
        }

        void Scripts::RebuildNative(bool clean) {
            InvokeBuild(Project::GetRootDirectory() / "Scripts" / "Native", clean);
        }
    }
}
