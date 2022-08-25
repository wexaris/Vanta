#pragma once

namespace Vanta {
    namespace IO {
        struct Image {
            unsigned char* Data = nullptr;
            uint32 Width, Height;
            uint Channels;

            operator bool() const { return Data != nullptr; }

            Image(const Path& path);
            ~Image();
        };
    }
}
