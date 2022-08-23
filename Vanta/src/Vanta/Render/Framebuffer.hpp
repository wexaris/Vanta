#pragma once

namespace Vanta {

    enum class FramebufferTextureFormat {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,

        // Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureParams
    {
        FramebufferTextureParams() = default;
        FramebufferTextureParams(FramebufferTextureFormat format)
            : TextureFormat(format) {}

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
        // TODO: Add filtering and wrap
    };

    struct FramebufferAttachmentParams {
        FramebufferAttachmentParams() = default;
        FramebufferAttachmentParams(std::initializer_list<FramebufferTextureParams> attachments)
            : Attachments(attachments) {}

        std::vector<FramebufferTextureParams> Attachments;
    };

    struct FramebufferParams {
        uint32_t Width = 0, Height = 0;
        FramebufferAttachmentParams Attachments;
        uint32_t Samples = 1;

        bool SwapChainTarget = false;
    };

    class Framebuffer {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void Resize(uint32 width, uint32 height) = 0;
        virtual int ReadPixel(uint32 attachmentIndex, int x, int y) = 0;

        virtual void ClearAttachment(uint32 attachmentIndex, int value) = 0;

        virtual uint32 GetColorAttachmentRendererID(uint32 attachmentIndex = 0) const = 0;

        virtual const FramebufferParams& GetParams() const = 0;

        static Ref<Framebuffer> Create(const FramebufferParams& params);

    protected:
        Framebuffer() = default;
    };
}
