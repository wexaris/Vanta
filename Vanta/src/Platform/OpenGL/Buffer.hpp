#pragma once
#include "Vanta/Render/Buffer.hpp"

namespace Vanta {

    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(usize size);
        OpenGLVertexBuffer(float* vertices, uint count);
        virtual ~OpenGLVertexBuffer();

        void Bind() const override;
        void Unbind() const override;

        const BufferLayout& GetLayout() const override      { return m_Layout; }
        void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

        void SetData(const void* data, usize size) override;

    private:
        friend class OpenGLVertexArray;

        uint m_RendererID = 0;
        BufferLayout m_Layout;
    };

    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer(uint* indices, uint count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual usize GetCount() const override { return m_Count; }

    private:
        friend class OpenGLVertexArray;

        uint m_RendererID = 0;
        uint m_Count;
    };
}