#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/IO/File.hpp"
#include "Vanta/Util/PlatformUtils.hpp"

namespace Vanta {
    namespace IO {
        File::File(const Path& path)
            : Filepath(Engine::Get().CorrectFilepath(path))
        {}

        std::string File::Read() const {
            std::string content;

            std::ifstream file(Filepath.c_str(), std::ios::in | std::ios::binary);
            if (file) {
                file.seekg(0, std::ios::end);
                auto size = file.tellg();
                if (size != -1) {
                    content.resize((size_t)file.tellg());
                    file.seekg(0, std::ios::beg);
                    file.read(&content[0], content.size());
                    file.close();
                }
                else VANTA_ERROR("Failed to read file: '{}'", Filepath);
            }
            else VANTA_ERROR("Failed to open file: '{}'", Filepath);

            return content;
        }

        void File::Write(const std::string& out) const {
            std::ofstream file(Filepath.c_str());
            if (file) {
                file << out;
            }
            else VANTA_ERROR("Failed to open file: '{}'", Filepath);
        }

        bool File::Exists() const {
            return std::filesystem::exists(Filepath);
        }

        Opt<File> FileDialog::OpenFile(const char* filter) {
            Path path = platform::OpenFileDialog(filter);
            if (path.empty())
                return None;
            return File(path);
        }

        Opt<File> FileDialog::SaveFile(const char* filter) {
            Path path = platform::SaveFileDialog(filter);
            if (path.empty())
                return None;
            return File(path);
        }
    }
}