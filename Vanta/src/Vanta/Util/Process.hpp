#pragma once

#include <string>
#include <vector>

#include <Vanta/Vanta.hpp>

namespace Vanta {

    struct CommandResult {
        int ExitCode = 0;
        std::string Output;
    };

    class Process {
    public:
        static CommandResult Run(const std::vector<std::string_view>& commandParts, const Path& workingDirectory);
    };
}
