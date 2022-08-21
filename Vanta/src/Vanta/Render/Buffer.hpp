#pragma once
#include "Vanta/Render/Shader.hpp"

namespace Vanta {

    class BufferLayout {
    public:
        struct Element {
            std::string Name;
            Shader::DataType Type;
            usize Size;
            usize Offset;
            bool Normalized;

            Element(Shader::DataType type, const std::string& name, bool normalized = false);
        };

        BufferLayout() = default;
        BufferLayout(const std::initializer_list<Element>& elements);

        const std::vector<Element>& GetElements() const { return m_Elements; }
        usize GetStride() const { return m_Stride; }

    private:
        std::vector<Element> m_Elements;
        usize m_Stride = 0;

        void CalcOffsetAndStride();
    };

    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout& layout) = 0;

        virtual void SetData(const void* data, usize size) = 0;

        static Ref<VertexBuffer> Create(usize size);
        static Ref<VertexBuffer> Create(float* vertices, uint count);

    protected:
        VertexBuffer() = default;
    };

    class IndexBuffer {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual usize GetCount() const = 0;

        static Ref<IndexBuffer> Create(uint* indices, uint count);

    protected:
        IndexBuffer() = default;
    };
}