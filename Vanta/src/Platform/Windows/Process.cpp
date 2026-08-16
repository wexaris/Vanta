#include "vantapch.hpp"
#include "Vanta/Util/Process.hpp"

#include <sstream>

namespace Vanta {

    namespace {
        std::string Quote(const std::string_view& value) {
            return std::string("\"") + std::string(value) + "\"";
        }

        std::string JoinCommand(const std::vector<std::string_view>& commandParts) {
            std::string command;
            for (usize i = 0; i < commandParts.size(); i++) {
                if (i > 0)
                    command += " ";
                command += commandParts[i];
            }
            return command;
        }
    }

    CommandResult Process::Run(const std::vector<std::string_view>& commandParts, const Path& workingDirectory) {
        std::stringstream output;
        int exitCode = Run(commandParts, workingDirectory, output);
        return { exitCode, output.str() };
    }

    int Process::Run(const std::vector<std::string_view>& commandParts, const Path& workingDirectory, std::ostream& outputStream) {
        if (commandParts.empty()) {
            VANTA_CORE_WARN("No command provided to run.");
            return 1;
        }

        const std::string command = JoinCommand(commandParts);

        std::string shellCommand = std::string("cmd /c \"cd /d ") + Quote(workingDirectory.string()) + " && " + command + " 2>&1\"";

        FILE* pipe = _popen(shellCommand.c_str(), "r");
        if (!pipe) {
            VANTA_CORE_ERROR("Failed to run command: {}", command);
            return 1;
        }

        std::array<char, 512> buffer{};
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
            outputStream << buffer.data();

        int exitCode = _pclose(pipe);
        return exitCode;
    }
}
