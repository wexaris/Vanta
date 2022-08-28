#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/IO/File.hpp"

namespace Vanta {
    namespace IO {
        File::File(const Path& path)
            : m_Filepath(Engine::Get().CorrectFilepath(path))
        {}

        std::string File::Read() const {
            std::string content;

            std::ifstream file(m_Filepath.c_str(), std::ios::in | std::ios::binary);
            if (file) {
                file.seekg(0, std::ios::end);
                auto size = file.tellg();
                if (size != -1) {
                    content.resize((size_t)file.tellg());
                    file.seekg(0, std::ios::beg);
                    file.read(&content[0], content.size());
                    file.close();
                }
                else VANTA_ERROR("Failed to read file: '{}'", m_Filepath);
            }
            else VANTA_ERROR("Failed to open file: '{}'", m_Filepath);

            return content;
        }

        void File::Write(const std::string& out) const {
            std::ofstream file(m_Filepath.c_str());
            if (file) {
                file << out;
            }
            else VANTA_ERROR("Failed to open file: '{}'", m_Filepath);
        }

        bool File::Exists() const {
            return std::filesystem::exists(m_Filepath);
        }
    }
}