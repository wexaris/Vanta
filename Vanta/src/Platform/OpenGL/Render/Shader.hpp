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
        using SourceMap = std::unordered_map<uint, std::string>;

        uint m_RendererID = 0;
        std::string m_Name;

        /// <summary>
        /// Compile the given shader sources.
        /// Automatically creates and saves the shader program.
        /// </summary>
        void Compile(SourceMap sources);

        /// <summary>
        /// Split a single shader source by the type of its parts.
        /// </summary>
        static SourceMap SplitSource(const std::string& source);

        /// <summary>
        /// Find the corresponding OpenGL shader type for the given type string.
        /// </summary>
        /// <returns>
        /// Returns a non-zero value, if the string matches a type.
        /// Returns a zero, if the string doesn't match any type. Doesn't log an error.
        /// </returns>
        static uint StringToShaderType(const std::string& str);
    };
}
