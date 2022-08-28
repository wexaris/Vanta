#pragma once

namespace Vanta {
    namespace IO {
        class File {
        public:
            const Path Filepath;
        
            File(const Path& path);
            ~File() = default;

            std::string Read() const;
            void Write(const std::string& out) const;

            bool Exists() const;
        };

        class FileDialog {
        public:
            static Opt<File> OpenFile(const char* filter);
            static Opt<File> SaveFile(const char* filter);
        };
    }
}