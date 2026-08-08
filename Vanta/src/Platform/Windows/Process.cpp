#include "vantapch.hpp"
#include "Vanta/Util/Process.hpp"

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
        if (commandParts.empty())
            return { 1, "No command provided" };

        const std::string command = JoinCommand(commandParts);

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
