#include "vantapch.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"
#include "Vanta/Render/Buffer.hpp"

#include "Platform/OpenGL/Buffer.hpp"

namespace Vanta {

    Ref<VertexBuffer> VertexBuffer::Create(usize size) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLVertexBuffer>(size);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }

    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint count) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLVertexBuffer>(vertices, count);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint* indices, uint count) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewBox<OpenGLIndexBuffer>(indices, count);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }

    BufferLayout::BufferLayout(const std::initializer_list<Element>& elements)
        : m_Elements(elements), m_Stride(0)
    {
        CalcOffsetAndStride();
    }

    void BufferLayout::CalcOffsetAndStride() {
        m_Stride = 0;
        usize offset = 0;
        for (auto& e : m_Elements) {
            e.Offset = offset;
            offset += e.Size;
            m_Stride += e.Size;
        }
    }
    
    BufferLayout::Element::Element(Shader::DataType type, const std::string& name, bool normalized)
        : Name(name), Type(type), Size(type.Size()), Offset(0), Normalized(normalized)
    {}
}
