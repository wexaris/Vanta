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
            ReadTo(content);
            return content;
        }

        std::vector<char> File::ReadBinary() const {
            std::vector<char> buffer;
            ReadTo(buffer);
            return buffer;
        }

        void File::Write(const std::string& data) const {
            Write(data.c_str(), data.size());
        }

        void File::Write(const char* data, usize count) const {
            std::ofstream file(Filepath.c_str(), std::ios::out | std::ios::binary);
            if (file) {
                file.write(data, count);
                file.flush();
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