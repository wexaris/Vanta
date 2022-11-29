#pragma once

namespace Vanta {
    namespace IO {
        class File {
        public:
            const Path Filepath;
        
            File(const Path& path);
            ~File() = default;

            std::string Read() const;
            std::vector<char> ReadBinary() const;

            void Write(const std::string& data) const;
            void Write(const char* data, usize count) const;

            void Append(const std::string& data) const;
            void Append(const char* data, usize count) const;

            template<typename Buff, typename T = char>
            void ReadTo(Buff& buffer) const {
                std::ifstream file(Filepath.c_str(), std::ios::in | std::ios::binary);
                if (file) {
                    file.seekg(0, std::ios::end);
                    auto size = file.tellg();
                    if (size != -1) {
                        file.seekg(0, std::ios::beg);
                        buffer.resize(size / sizeof(T));
                        file.read((char*)buffer.data(), size);
                    }
                    else VANTA_ERROR("Failed to read file: '{}'", Filepath);
                }
                else VANTA_ERROR("Failed to open file: '{}'", Filepath);
            }

            template<typename T>
            void ReadTo(std::vector<T>& buffer) const {
                ReadTo<std::vector<T>, T>(buffer);
            }

            void ReadTo(std::string& buffer) const {
                ReadTo<std::string, char>(buffer);
            }

            bool Exists() const;
        };

        class FileDialog {
        public:
            static Opt<File> OpenFile(const char* filter);
            static Opt<File> SaveFile(const char* filter);
        };
    }
}