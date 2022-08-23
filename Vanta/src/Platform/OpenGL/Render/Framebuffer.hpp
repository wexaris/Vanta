#pragma once
#include "Vanta/Render/Framebuffer.hpp"

namespace Vanta {

    class OpenGLFramebuffer : public Framebuffer {
    public:
        OpenGLFramebuffer(const FramebufferParams& params);
        ~OpenGLFramebuffer();

        void Bind() const override;
        void Unbind() const override;

        void Invalidate();

        void Resize(uint32 width, uint32 height) override;
        int ReadPixel(uint32 attachmentIndex, int x, int y) override;

        void ClearAttachment(uint32 attachmentIndex, int value) override;

        uint32 GetColorAttachmentRendererID(uint32 attachmentIndex = 0) const override {
            VANTA_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Framebuffer color attachment index invalid");
            return m_ColorAttachments[attachmentIndex];
        }

        virtual const FramebufferParams& GetParams() const override { return m_Params; }

    private:
        static constexpr uint32 MAX_FRAMEFUFFER_SIZE = 8192;

        uint m_RendererID = 0;
        FramebufferParams m_Params;

        std::vector<uint32> m_ColorAttachments;
        std::vector<FramebufferTextureParams> m_ColorAttachmentParams;

        uint32 m_DepthAttachment = 0;
        FramebufferTextureParams m_DepthAttachmentParams = FramebufferTextureFormat::None;
    };
}
