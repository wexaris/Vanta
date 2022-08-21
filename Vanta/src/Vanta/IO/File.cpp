#include "vantapch.hpp"
#include "Vanta/IO/File.hpp"

namespace Vanta {
    namespace IO {
        File::File(const Path& path) {
            std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
            if (file) {
                file.seekg(0, std::ios::end);
                auto size = file.tellg();
                if (size != -1) {
                    Content.resize((size_t)file.tellg());
                    file.seekg(0, std::ios::beg);
                    file.read(&Content[0], Content.size());
                    file.close();
                }
                else VANTA_ERROR("Failed to read file: '{}'", path);
            }
            else VANTA_ERROR("Failed to open file: '{}'", path);
        }
    }
}