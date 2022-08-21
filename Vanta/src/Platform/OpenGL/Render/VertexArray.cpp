#include "vantapch.hpp"
#include "Platform/OpenGL/Render/Buffer.hpp"
#include "Platform/OpenGL/Render/VertexArray.hpp"

#include <glad/glad.h>

namespace Vanta {

    static GLenum OpenGLDataType(Shader::DataType type) {
        switch (type)
        {
        case Shader::DataType::Int:    return GL_INT;
        case Shader::DataType::Int2:   return GL_INT;
        case Shader::DataType::Int3:   return GL_INT;
        case Shader::DataType::Int4:   return GL_INT;
        case Shader::DataType::UInt:   return GL_UNSIGNED_INT;
        case Shader::DataType::UInt2:  return GL_UNSIGNED_INT;
        case Shader::DataType::UInt3:  return GL_UNSIGNED_INT;
        case Shader::DataType::UInt4:  return GL_UNSIGNED_INT;
        case Shader::DataType::Float:  return GL_FLOAT;
        case Shader::DataType::Float2: return GL_FLOAT;
        case Shader::DataType::Float3: return GL_FLOAT;
        case Shader::DataType::Float4: return GL_FLOAT;
        case Shader::DataType::Mat3:   return GL_FLOAT;
        case Shader::DataType::Mat4:   return GL_FLOAT;
        case Shader::DataType::Bool:   return GL_BOOL;
        default:
            VANTA_UNREACHABLE("Invalid ShaderDataType!");
            return 0;
        }
    }

    OpenGLVertexArray::OpenGLVertexArray() {
        VANTA_PROFILE_RENDER_FUNCTION();
        glCreateVertexArrays(1, &m_RendererID);
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        VANTA_PROFILE_RENDER_FUNCTION();
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind() const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind() const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
        VANTA_PROFILE_RENDER_FUNCTION();

        VANTA_CORE_ASSERT(!vertexBuffer->GetLayout().GetElements().empty(), "Vertex buffer missing layout!");

        auto vbo = RefCast<OpenGLVertexBuffer>(vertexBuffer);

        const auto& layout = vertexBuffer->GetLayout();

        for (const auto& e : layout.GetElements()) {
            uint itemCount = e.Type.ItemCount();
            GLenum type = OpenGLDataType(e.Type);
            GLboolean normalized = e.Normalized ? GL_TRUE : GL_FALSE;
           
            if (e.Type == Shader::DataType::Mat3 || e.Type == Shader::DataType::Mat4) {
                for (usize i = 0; i < itemCount; i++) {
                    auto offset = i * itemCount * sizeof(float);

                    glEnableVertexArrayAttrib(m_RendererID, m_VertexBufferIndex);
                    glVertexArrayVertexBuffer(m_RendererID, m_VertexBufferIndex, vbo->m_RendererID, offset, (uint)layout.GetStride());
                    glVertexArrayAttribFormat(m_RendererID, m_VertexBufferIndex, itemCount, type, normalized, 0);

                    glVertexAttribDivisor(m_VertexBufferIndex, 1);
                    glVertexArrayAttribBinding(m_RendererID, m_VertexBufferIndex, m_VertexBufferIndex);
                }
            }
            else {
                glEnableVertexArrayAttrib(m_RendererID, m_VertexBufferIndex);
                glVertexArrayVertexBuffer(m_RendererID, m_VertexBufferIndex, vbo->m_RendererID, e.Offset, (uint)layout.GetStride());
                glVertexArrayAttribFormat(m_RendererID, m_VertexBufferIndex, itemCount, type, normalized, 0);

                glVertexArrayAttribBinding(m_RendererID, m_VertexBufferIndex, m_VertexBufferIndex);
            }

            m_VertexBufferIndex++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
        VANTA_PROFILE_RENDER_FUNCTION();

        auto buffer = std::static_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
        glVertexArrayElementBuffer(m_RendererID, buffer->m_RendererID);

        m_IndexBuffer = indexBuffer;
    }
}
