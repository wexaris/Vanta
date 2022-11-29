#pragma once
#include "Vanta/Core/Buffer.hpp"

#include <filewatch/FileWatch.hpp>

namespace Vanta {
    namespace IO {

        using FileWatcher = filewatch::FileWatch<std::string>;

        class File {
        public:
            const Path Filepath;
        
            File(const Path& path);
            ~File() = default;

            std::string Read() const;
            Buffer ReadBytes() const;

            void Write(const std::string& data) const;
            void Write(const char* data, usize count) const;

            void Append(const std::string& data) const;
            void Append(const char* data, usize count) const;

            bool Exists() const;
        };

        class FileDialog {
        public:
            static Opt<File> OpenFile(const char* filter);
            static Opt<File> SaveFile(const char* filter);
        };
    }
}