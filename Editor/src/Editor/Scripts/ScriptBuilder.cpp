#include "ScriptBuilder.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include <Vanta/Util/Process.hpp>
#include <Vanta/Project/Project.hpp>

namespace Vanta {
    namespace Editor {

        namespace {
            template<typename T>
            std::string Quote(const T& value) {
                return std::string("\"") + value + "\"";
            }

            template<>
            std::string Quote(const Path& value) {
                return std::string("\"") + value.string() + "\"";
            }
        }

        bool NativeScriptBuilder::Build(const ScriptBuildOptions& options) const {
            const Path scriptCoreDirectory = Engine::RuntimeScriptDirectory(Scripts::ScriptType::Native);
            const Path scriptDirectory = Project::GetScriptDirectory(Scripts::ScriptType::Native);
            const Path buildDirectory = scriptDirectory / "build";
            if (options.Clean)
                std::filesystem::remove_all(buildDirectory);

            std::filesystem::create_directories(buildDirectory);

            CommandResult configure = Process::Run({
                "cmake",
                "-S", Quote(scriptDirectory),
                "-B", Quote(buildDirectory),
                std::string("-DCMAKE_BUILD_TYPE:STRING=") + ToCMakeBuildType(options.Configuration),
                FMT("-DVANTA_LIB:STRING={}", Quote(scriptCoreDirectory)),
                FMT("-DVANTA_INCLUDE:STRING={}", Quote(scriptCoreDirectory / "include"))
            }, scriptDirectory);

            VANTA_INFO("Configuring native script project \n{}", configure.Output);

            if (configure.ExitCode != 0) {
                VANTA_CORE_ERROR("Native script configure failed (code {})", configure.ExitCode);
                return false;
            }

            CommandResult build = Process::Run({
                "cmake",
                "--build", Quote(buildDirectory),
                "--config", ToCMakeBuildType(options.Configuration)
            }, scriptDirectory);

            VANTA_INFO("Building native script project \n{}", build.Output);

            if (build.ExitCode != 0) {
                VANTA_CORE_ERROR("Native script build failed (code {})", build.ExitCode);
                return false;
            }

            return true;
        }

        bool CSharpScriptBuilder::Build(const ScriptBuildOptions& options) const {
            const Path scriptCoreDirectory = Engine::RuntimeScriptDirectory(Scripts::ScriptType::CSharp);
            const Path scriptDirectory = Project::GetScriptDirectory(Scripts::ScriptType::CSharp);
            const Path projectFile = scriptDirectory / "Scripts_CSharp.csproj";
            const Path outputDirectory = scriptDirectory / "Binaries";
            if (options.Clean)
                std::filesystem::remove_all(outputDirectory);

            std::filesystem::create_directories(outputDirectory);

            CommandResult build = Process::Run({
                "dotnet",
                "build", Quote(projectFile),
                "--configuration", ToDotnetConfiguration(options.Configuration),
                "--output", Quote(outputDirectory),
                FMT("-p:VantaScriptCoreDir={}", Quote(scriptCoreDirectory))
            }, scriptDirectory);

            VANTA_INFO("Building C# script project \n{}", build.Output);

            if (build.ExitCode != 0) {
                VANTA_CORE_ERROR("C# script build failed (code {})", build.ExitCode);
                return false;
            }

            return true;
        }

    }
}