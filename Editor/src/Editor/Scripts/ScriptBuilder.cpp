#include "ScriptBuilder.hpp"

#include <array>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

namespace Vanta {
    namespace Editor {

        namespace {
            struct CommandResult {
                int ExitCode = 0;
                std::string Output;
            };

            static std::string Quote(const std::string& value) {
                return std::string("\"") + value + "\"";
            }

            static CommandResult RunCommand(const std::vector<std::string_view>& commandParts, const Path& workingDirectory) {
                std::string command;
                for (usize i = 0; i < commandParts.size(); i++) {
                    if (i > 0)
                        command += " ";
                    command += commandParts[i];
                }

                // Keep the build execution in the script project directory for deterministic relative paths.
                std::string shellCommand = std::string("cmd /c \"cd /d ") + Quote(workingDirectory.string()) + " && " + command + " 2>&1\"";

                std::array<char, 512> buffer{};
                std::string output;

                FILE* pipe = _popen(shellCommand.c_str(), "r");
                if (!pipe)
                    return { 1, "Failed to launch process" };

                while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
                    output += buffer.data();

                int exitCode = _pclose(pipe);
                return { exitCode, output };
            }
        }

        bool NativeScriptBuilder::Build(const Path& projectDirectory, const ScriptBuildOptions& options) const {
            const Path buildDirectory = projectDirectory / "build";
            if (options.Clean)
                std::filesystem::remove_all(buildDirectory);

            std::filesystem::create_directories(buildDirectory);

            CommandResult configure = RunCommand({
                "cmake",
                "-S", Quote(projectDirectory.string()),
                "-B", Quote(buildDirectory.string()),
                std::string("-DCMAKE_BUILD_TYPE:STRING=") + ToCMakeBuildType(options.Configuration)
            }, projectDirectory);

            if (configure.ExitCode != 0) {
                VANTA_CORE_ERROR("Native script configure failed (code {}): {}", configure.ExitCode, configure.Output);
                return false;
            }

            CommandResult build = RunCommand({
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

            CommandResult build = RunCommand({
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