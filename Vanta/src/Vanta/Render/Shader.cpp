#include "vantapch.hpp"
#include "Vanta/Render/Shader.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"

#include "Platform/OpenGL/Render/Shader.hpp"

namespace Vanta {

    Ref<Shader> Shader::Create(const Path& path) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewRef<OpenGLShader>(path);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }

    Ref<Shader> Shader::Create(const std::string& name, const Path& filepath) {
        switch (GraphicsAPI::GetAPI()) {
        case GraphicsAPI::OpenGL: return NewRef<OpenGLShader>(name, filepath);
        default:
            VANTA_UNREACHABLE("Invalid graphics API!");
            return nullptr;
        }
    }
    
    void ShaderLibrary::Add(const Ref<Shader>& shader) {
        auto& name = shader->GetName();
        VANTA_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader already exists: {}", name);
        m_Shaders[name] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& filepath) {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }
    
    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
        auto shader = Shader::Create(name, filepath);
        Add(shader);
        return shader;
    }
    
    Ref<Shader> ShaderLibrary::Get(const std::string& name) {
        VANTA_ASSERT(Exists(name), "Shader not found: {}", name);
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) {
        return m_Shaders.find(name) != m_Shaders.end();
    }

    uint Shader::DataType::ItemCount() const {
        switch (m_Type)
        {
        case Type::Int:    return 1;
        case Type::Int2:   return 2;
        case Type::Int3:   return 3;
        case Type::Int4:   return 4;
        case Type::UInt:   return 1;
        case Type::UInt2:  return 2;
        case Type::UInt3:  return 3;
        case Type::UInt4:  return 4;
        case Type::Float:  return 1;
        case Type::Float2: return 2;
        case Type::Float3: return 3;
        case Type::Float4: return 4;
        case Type::Mat3:   return 3; // 3 * float3
        case Type::Mat4:   return 4; // 4 * float4
        case Type::Bool:   return 1;
        default:
            VANTA_UNREACHABLE("Invalid ShaderDataType!");
            return 0;
        }
    }

    uint Shader::DataType::Size() const {
        switch (m_Type)
        {
        case Type::Int:    return 4;
        case Type::Int2:   return 4 * 2;
        case Type::Int3:   return 4 * 3;
        case Type::Int4:   return 4 * 4;
        case Type::UInt:   return 4;
        case Type::UInt2:  return 4 * 2;
        case Type::UInt3:  return 4 * 3;
        case Type::UInt4:  return 4 * 4;
        case Type::Float:  return 4;
        case Type::Float2: return 4 * 2;
        case Type::Float3: return 4 * 3;
        case Type::Float4: return 4 * 4;
        case Type::Mat3:   return 4 * 3 * 3;
        case Type::Mat4:   return 4 * 4 * 4;
        case Type::Bool:   return 1;
        default:
            VANTA_UNREACHABLE("Invalid ShaderDataType!");
            return 0;
        }
    }
}
