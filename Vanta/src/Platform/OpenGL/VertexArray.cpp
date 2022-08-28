#include "vantapch.hpp"
#include "Platform/OpenGL/Buffer.hpp"
#include "Platform/OpenGL/VertexArray.hpp"

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
           
            switch (e.Type) {
            case Shader::DataType::Float:  [[fallthrough]];
            case Shader::DataType::Float2: [[fallthrough]];
            case Shader::DataType::Float3: [[fallthrough]];
            case Shader::DataType::Float4: {
                glEnableVertexArrayAttrib(m_RendererID, m_VertexBufferIndex);
                glVertexArrayVertexBuffer(m_RendererID, m_VertexBufferIndex, vbo->m_RendererID, e.Offset, (uint)layout.GetStride());
                glVertexArrayAttribFormat(m_RendererID, m_VertexBufferIndex, itemCount, type, normalized, 0);

                glVertexArrayAttribBinding(m_RendererID, m_VertexBufferIndex, m_VertexBufferIndex);
                m_VertexBufferIndex++;
                break;
            }

            case Shader::DataType::Int:   [[fallthrough]];
            case Shader::DataType::Int2:  [[fallthrough]];
            case Shader::DataType::Int3:  [[fallthrough]];
            case Shader::DataType::Int4:  [[fallthrough]];
            case Shader::DataType::UInt:  [[fallthrough]];
            case Shader::DataType::UInt2: [[fallthrough]];
            case Shader::DataType::UInt3: [[fallthrough]];
            case Shader::DataType::UInt4: [[fallthrough]];
            case Shader::DataType::Bool: {
                glEnableVertexArrayAttrib(m_RendererID, m_VertexBufferIndex);
                glVertexArrayVertexBuffer(m_RendererID, m_VertexBufferIndex, vbo->m_RendererID, e.Offset, (uint)layout.GetStride());
                glVertexArrayAttribIFormat(m_RendererID, m_VertexBufferIndex, itemCount, type, 0);

                glVertexArrayAttribBinding(m_RendererID, m_VertexBufferIndex, m_VertexBufferIndex);
                m_VertexBufferIndex++;
                break;
            }

            case Shader::DataType::Mat3: [[fallthrough]];
            case Shader::DataType::Mat4: {
                for (uint i = 0; i < itemCount; i++) {
                    auto offset = i * itemCount * sizeof(float);

                    glEnableVertexArrayAttrib(m_VertexBufferIndex, m_VertexBufferIndex);

                    glEnableVertexArrayAttrib(m_RendererID, m_VertexBufferIndex);
                    glVertexArrayVertexBuffer(m_RendererID, m_VertexBufferIndex, vbo->m_RendererID, offset, (GLsizei)layout.GetStride());
                    glVertexArrayAttribFormat(m_RendererID, m_VertexBufferIndex, itemCount, type, normalized, 0);

                    glVertexAttribDivisor(m_VertexBufferIndex, 1);
                    glVertexArrayAttribBinding(m_RendererID, m_VertexBufferIndex, m_VertexBufferIndex);

                    glEnableVertexArrayAttrib(m_RendererID, m_VertexBufferIndex);
                    glVertexArrayAttribFormat(m_RendererID, m_VertexBufferIndex, itemCount, type, normalized, (GLuint)e.Offset);
                    glVertexArrayBindingDivisor(m_RendererID, m_VertexBufferIndex, 1);
                    m_VertexBufferIndex++;
                }
                break;
            }

            default:
                VANTA_UNREACHABLE("Invalid shader data type!");
                break;
            }
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
