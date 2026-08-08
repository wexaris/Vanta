#include "ScriptBuilder.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include <Vanta/Util/Process.hpp>

namespace Vanta {
    namespace Editor {

        namespace {
            std::string Quote(const std::string& value) {
                return std::string("\"") + value + "\"";
            }
        }

        bool NativeScriptBuilder::Build(const Path& projectDirectory, const ScriptBuildOptions& options) const {
            const Path buildDirectory = projectDirectory / "build";
            if (options.Clean)
                std::filesystem::remove_all(buildDirectory);

            std::filesystem::create_directories(buildDirectory);

            CommandResult configure = Process::Run({
                "cmake",
                "-S", Quote(projectDirectory.string()),
                "-B", Quote(buildDirectory.string()),
                std::string("-DCMAKE_BUILD_TYPE:STRING=") + ToCMakeBuildType(options.Configuration)
            }, projectDirectory);

            if (configure.ExitCode != 0) {
                VANTA_CORE_ERROR("Native script configure failed (code {}): {}", configure.ExitCode, configure.Output);
                return false;
            }

            CommandResult build = Process::Run({
                "cmake",
                "--build", Quote(buildDirectory.string()),
                "--config", ToCMakeBuildType(options.Configuration)
            }, projectDirectory);

            if (build.ExitCode != 0) {
                VANTA_CORE_ERROR("Native script build failed (code {}): {}", build.ExitCode, build.Output);
                return false;
            }

            return true;
        }

        bool CSharpScriptBuilder::Build(const Path& projectDirectory, const ScriptBuildOptions& options) const {
            const Path outputDirectory = projectDirectory / "Binaries";
            if (options.Clean)
                std::filesystem::remove_all(outputDirectory);

            std::filesystem::create_directories(outputDirectory);

            const Path projectFile = projectDirectory / "Scripts_CSharp.csproj";

            CommandResult build = Process::Run({
                "dotnet",
                "build", Quote(projectFile.string()),
                "--configuration", ToDotnetConfiguration(options.Configuration),
                "--output", Quote(outputDirectory.string())
            }, projectDirectory);

            if (build.ExitCode != 0) {
                VANTA_CORE_ERROR("C# script build failed (code {}): {}", build.ExitCode, build.Output);
                return false;
            }

            return true;
        }

    }
}