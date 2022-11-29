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
            ScopedBuffer buffer = ReadBytes();
            std::string string(buffer.As<char>(), buffer.Size());
            return string;
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

        void File::Append(const std::string& data) const {
            Append(data.c_str(), data.size());
        }

        void File::Append(const char* data, usize count) const {
            std::ofstream file(Filepath.c_str(), std::ios::app);
            if (file) {
                file.write(data, count);
                file.flush();
            }
            else VANTA_ERROR("Failed to open file: '{}'", Filepath);
        }

        Buffer File::ReadBytes() const {
            std::ifstream file(Filepath.c_str(), std::ios::binary | std::ios::ate);
            if (!file) {
                VANTA_ERROR("Failed to open file: '{}'", Filepath);
                return Buffer();
            }

            auto end = file.tellg();
            file.seekg(0, std::ios::beg);
            auto size = end - file.tellg();

            if (size == 0) {
                // File empty
                return Buffer();
            }

            Buffer buffer(size);
            file.read(buffer.As<char>(), size);
            file.close();

            return buffer;
        }

        bool File::Exists() const {
            return std::filesystem::exists(Filepath);
        }

        Opt<File> FileDialog::OpenFile(const char* filter) {
            Path path = Platform::OpenFileDialog(filter);
            if (path.empty())
                return None;
            return File(path);
        }

        Opt<File> FileDialog::SaveFile(const char* filter) {
            Path path = Platform::SaveFileDialog(filter);
            if (path.empty())
                return None;
            return File(path);
        }
    }
}