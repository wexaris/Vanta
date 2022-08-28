#pragma once

namespace Vanta {
    namespace IO {
        class File {
        public:
            File(const Path& path);
            ~File() = default;

            std::string Read() const;
            void Write(const std::string& out) const;

            bool Exists() const;

        private:
            Path m_Filepath;
        };
    }
}