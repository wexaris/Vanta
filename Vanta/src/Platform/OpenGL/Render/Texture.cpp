#include "vantapch.hpp"
#include "Platform/OpenGL/Render/Texture.hpp"

#include <glad/glad.h>

namespace Vanta {
    OpenGLTexture2D::OpenGLTexture2D(uint32 width, uint32 height) :
        m_Width(width),
        m_Height(height),
        m_InternalFormat(GL_RGBA8),
        m_DataFormat(GL_RGBA)
    {
        VANTA_PROFILE_RENDER_FUNCTION();

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

        // Create texture storage
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

        // Set wrapping mode
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        // Set filtering mode
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    OpenGLTexture2D::OpenGLTexture2D(const Path& path) {
        VANTA_PROFILE_RENDER_FUNCTION();

        auto image = IO::Image(path);

        m_Width = image.Width;
        m_Height = image.Height;

        if (image.Channels == 4) {
            m_InternalFormat = GL_RGBA8;
            m_DataFormat = GL_RGBA;
        }
        else if (image.Channels == 3) {
            m_InternalFormat = GL_RGB8;
            m_DataFormat = GL_RGB;
        }

        VANTA_ASSERT(m_InternalFormat & m_DataFormat, "Unsupported image format: '{}'", path.string());

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

        // Create texture storage
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, image.Width, image.Height);

        // Set wrapping mode
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        // Set filtering mode
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, image.Data);
        //glGenerateTextureMipmap(m_RendererID);
    }

    OpenGLTexture2D::~OpenGLTexture2D() {
        VANTA_PROFILE_RENDER_FUNCTION();
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::Bind(uint slot) const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glBindTextureUnit(slot, m_RendererID);
    }

    void OpenGLTexture2D::SetData(const void* data, usize size) {
        VANTA_PROFILE_RENDER_FUNCTION();
        auto pixel = m_DataFormat == GL_RGBA ? 4 : 3;
        VANTA_ASSERT(size == (m_Width * m_Height * pixel), "Image data doesn't match texture properties!");
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
        //glGenerateTextureMipmap(m_RendererID);
    }
}
