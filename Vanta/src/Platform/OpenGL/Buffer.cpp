#include "vantapch.hpp"
#include "Platform/OpenGL/Buffer.hpp"

#include <glad/glad.h>

namespace Vanta {

    OpenGLVertexBuffer::OpenGLVertexBuffer(usize size) {
        VANTA_PROFILE_RENDER_FUNCTION();

        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint count) {
        VANTA_PROFILE_RENDER_FUNCTION();

        VANTA_CORE_ASSERT(vertices, "Vertex pointer is null!");

        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, count * sizeof(float), vertices, GL_STATIC_DRAW);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() {
        VANTA_PROFILE_RENDER_FUNCTION();
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLVertexBuffer::Bind() const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLVertexBuffer::Unbind() const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::SetData(const void* data, usize size) {
        VANTA_PROFILE_RENDER_FUNCTION();
        glNamedBufferSubData(m_RendererID, 0, size, data);
    }


    OpenGLIndexBuffer::OpenGLIndexBuffer(uint* indices, uint count)
        : m_Count(count)
    {
        VANTA_PROFILE_RENDER_FUNCTION();

        VANTA_CORE_ASSERT(indices, "Vertex pointer is null!");

        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, count * sizeof(uint), indices, GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        VANTA_PROFILE_RENDER_FUNCTION();
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLIndexBuffer::Bind() const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLIndexBuffer::Unbind() const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
