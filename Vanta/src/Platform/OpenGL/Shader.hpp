#pragma once
#include "Vanta/Render/Shader.hpp"

namespace Vanta {

    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const Path& path);
        OpenGLShader(const std::string& name, const Path& filepath);
        virtual ~OpenGLShader();

        void Bind() const override;
        void Unbind() const override;

        const std::string& GetName() const override { return m_Name; }

        void SetInt(const std::string& name, int value) override;
        void SetUInt(const std::string& name, uint value) override;
        void SetIntArray(const std::string& name, int* values, uint count) override;
        void SetUIntArray(const std::string& name, uint* values, uint count) override;

        void SetFloat(const std::string& name, float value) override;
        void SetFloat2(const std::string& name, const glm::vec2& values) override;
        void SetFloat3(const std::string& name, const glm::vec3& values) override;
        void SetFloat4(const std::string& name, const glm::vec4& values) override;

        void SetMat3(const std::string& name, const glm::mat3& matrix) override;
        void SetMat4(const std::string& name, const glm::mat4& matrix) override;

    private:
        uint m_RendererID = 0;
        std::string m_Name;
        Path m_Filepath;

        std::unordered_map<uint, std::vector<uint32>> m_VulkanSPIRV;
        std::unordered_map<uint, std::vector<uint32>> m_OpenGLSPIRV;

        std::unordered_map<uint, std::string> m_OpenGLSources;

        /// <summary>
        /// Split a single shader source by the type of its parts.
        /// </summary>
        std::unordered_map<uint, std::string> PreProcess(const std::string& source);

        void CompileOrLoadVulkanBinaries(const std::unordered_map<uint, std::string>& sources);
        void CompileOrLoadOpenGLBinaries();

        void CompileOpenGLSources(const std::unordered_map<uint, std::string>& sources);

        /// <summary>
        /// Compile the given shader sources.
        /// Automatically creates and saves the shader program.
        /// </summary>
        void CreateProgram();

        void Reflect(uint, const std::vector<uint32>& data);
    };
}
