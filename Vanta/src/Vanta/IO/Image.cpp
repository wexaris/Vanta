#include "vantapch.hpp"
#include "Vanta/IO/Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Vanta {
    namespace IO {
        Image::Image(const Path& path) {
            int width, height, channels;
            stbi_set_flip_vertically_on_load(1);
            Data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
            Width = Data ? width : 0;
            Height = Data ? height : 0;
            Channels = Data ? channels : 0;

            if (!Data) {
                VANTA_CRITICAL("Failed to load image: {}", path);
            }
        }

        Image::~Image() {
            if (Data)
                stbi_image_free(Data);
        }
    }
}
