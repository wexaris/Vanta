#pragma once

namespace Vanta {
    namespace IO {
        struct File {
            std::string Content;

            File(const Path& path);
            ~File() = default;
        };
    }
}