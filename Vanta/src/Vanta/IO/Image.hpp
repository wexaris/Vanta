#pragma once

namespace Vanta {
    namespace IO {
        struct Image {
            unsigned char* Data = nullptr;
            uint32 Width, Height;
            uint Channels;

            Image(const Path& path);
            ~Image();
        };
    }
}
