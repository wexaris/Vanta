#include "vantapch.hpp"
#include "Platform/OpenGL/Render/Framebuffer.hpp"

#include <glad/glad.h>

namespace Vanta {

    namespace detail {

        GLenum TextureTarget(bool multisampled) {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count) {
            glCreateTextures(TextureTarget(multisampled), count, outID);
        }

        void BindTexture(bool multisampled, uint32_t id) {
            glBindTexture(TextureTarget(multisampled), id);
        }

        void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) {
            bool multisampled = samples > 1;
            if (multisampled) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
            }
            else {
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
        }

        void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
            bool multisampled = samples > 1;
            if (multisampled) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
            }
            else {
                glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
        }

        void CreateTexturesV(bool multisampled, uint32_t* outID, uint32_t count) {
            glCreateTextures(TextureTarget(multisampled), count, outID);
        }

        void AttachColorTextureV(uint32 framebufferID, uint32 texID, int samples, GLenum internalFormat, GLenum /*format*/, uint32_t width, uint32_t height, int index) {
            bool multisampled = samples > 1;
            if (multisampled) {
                glTextureStorage2DMultisample(texID, samples, internalFormat, width, height, GL_FALSE);
            }
            else {
                glTextureStorage2D(texID, 1, internalFormat, width, height);

                glTextureParameteri(texID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTextureParameteri(texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }

            glNamedFramebufferTexture(framebufferID, GL_COLOR_ATTACHMENT0 + index, texID, 0);
        }

        void AttachDepthTextureV(uint32 framebufferID, uint32 texID, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
            bool multisampled = samples > 1;
            if (multisampled) {
                glTextureStorage2DMultisample(texID, samples, format, width, height, GL_FALSE);
            }
            else {
                glTextureStorage2D(texID, 1, format, width, height);

                glTextureParameteri(texID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTextureParameteri(texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }

            glNamedFramebufferTexture(framebufferID, attachmentType, texID, 0);
        }

        bool IsDepthFormat(FramebufferTextureFormat format) {
            switch (format) {
            case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
            default: return false;
            }
        }

        GLenum GLTextureFormat(FramebufferTextureFormat format) {
            switch (format) {
            case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
            case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
            default:
                VANTA_UNREACHABLE("Invalid OpenGL texture format");
                return 0;
            }
        }
    }

    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferParams& params) 
        : m_Params(params)
    {
        for (auto spec : m_Params.Attachments.Attachments) {
            if (!detail::IsDepthFormat(spec.TextureFormat))
                m_ColorAttachmentParams.emplace_back(spec);
            else
                m_DepthAttachmentParams = spec;
        }

        Invalidate();
    }

    OpenGLFramebuffer::~OpenGLFramebuffer() {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }

    void OpenGLFramebuffer::Bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Params.Width, m_Params.Height);
    }

    void OpenGLFramebuffer::Unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Invalidate() {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);

            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
        }

        glCreateFramebuffers(1, &m_RendererID);
        //glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        bool multisample = m_Params.Samples > 1;

        // Attachments
        if (m_ColorAttachmentParams.size()) {
            m_ColorAttachments.resize(m_ColorAttachmentParams.size());
            detail::CreateTexturesV(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

            for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
                //detail::BindTexture(multisample, m_ColorAttachments[i]);
                switch (m_ColorAttachmentParams[i].TextureFormat) {
                case FramebufferTextureFormat::RGBA8:
                    detail::AttachColorTextureV(m_RendererID, m_ColorAttachments[i], m_Params.Samples, GL_RGBA8, GL_RGBA, m_Params.Width, m_Params.Height, i);
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    detail::AttachColorTextureV(m_RendererID, m_ColorAttachments[i], m_Params.Samples, GL_R32I, GL_RED_INTEGER, m_Params.Width, m_Params.Height, i);
                    break;
                default:
                    VANTA_UNREACHABLE("Invalid depth texture format!");
                    break;
                }
            }
        }

        if (m_DepthAttachmentParams.TextureFormat != FramebufferTextureFormat::None) {
            detail::CreateTexturesV(multisample, &m_DepthAttachment, 1);
            //detail::BindTexture(multisample, m_DepthAttachment);
            switch (m_DepthAttachmentParams.TextureFormat) {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                detail::AttachDepthTextureV(m_RendererID, m_DepthAttachment, m_Params.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Params.Width, m_Params.Height);
                break;
            default:
                VANTA_UNREACHABLE("Invalid depth texture format!");
                break;
            }
        }

        if (m_ColorAttachments.size() > 1) {
            VANTA_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Only 4 color attachments are supported!");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glNamedFramebufferDrawBuffers(m_RendererID, m_ColorAttachments.size(), buffers);
        }
        else if (m_ColorAttachments.empty()) {
            // Only depth-pass
            glNamedFramebufferDrawBuffer(m_RendererID, GL_NONE);
        }

        VANTA_CORE_ASSERT(glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Resize(uint32 width, uint32 height) {
        if (width == 0 || height == 0 || width > MAX_FRAMEFUFFER_SIZE || height > MAX_FRAMEFUFFER_SIZE) {
            VANTA_CORE_WARN("Attempted to resize framebuffer to {}, {}", width, height);
            return;
        }
        m_Params.Width = width;
        m_Params.Height = height;

        Invalidate();
    }

    int OpenGLFramebuffer::ReadPixel(uint32 attachmentIndex, int x, int y) {
        VANTA_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Framebuffer color attachment index invalid");

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
        return pixelData;
    }

    void OpenGLFramebuffer::ClearAttachment(uint32 attachmentIndex, int value) {
        VANTA_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Framebuffer color attachment index invalid");

        auto& spec = m_ColorAttachmentParams[attachmentIndex];
        glClearTexImage(m_ColorAttachments[attachmentIndex], 0, detail::GLTextureFormat(spec.TextureFormat), GL_INT, &value);
    }
}
