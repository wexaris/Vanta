#pragma once
#include "Vanta/Render/Texture.hpp"

namespace Vanta {

    class OpenGLTexture2D : public Texture2D {
    public:
        OpenGLTexture2D(uint32 width, uint32 height);
        OpenGLTexture2D(const Path& path);
        ~OpenGLTexture2D();

        void Bind(uint slot) const override;

        void SetData(const void* data, usize size) override;

        bool IsValid() const override { return m_RendererID != 0; }

        uint32 GetWidth() const override  { return m_Width; }
        uint32 GetHeight() const override { return m_Height; }

        uint32 GetRendererID() const override { return m_RendererID; };

        bool operator==(const Texture& other) const override {
            return m_RendererID == reinterpret_cast<const OpenGLTexture2D&>(other).m_RendererID;
        }
        bool operator!=(const Texture& other) const override {
            return m_RendererID != reinterpret_cast<const OpenGLTexture2D&>(other).m_RendererID;
        }

    private:
        uint m_RendererID = 0;
        uint32 m_Width = 0;
        uint32 m_Height = 0;
        uint m_InternalFormat = 0;
        uint m_DataFormat = 0;
    };
}
