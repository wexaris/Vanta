#include "vantapch.hpp"
#include "Platform/OpenGL/Shader.hpp"

#include <glad/glad.h>

namespace Vanta {

    std::string GetShaderInfoLog(uint shader) {
        // Get length of log messages
        GLint msgLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &msgLength);
        if (msgLength > 0) {
            // Get log message
            std::string log; 
            log.resize(msgLength);
            glGetShaderInfoLog(shader, msgLength, &msgLength, log.data());
            // Remove any trailing whitespace
            return log.substr(0, log.find_last_not_of("\t\r\n\f\v") + 1);
        }
        return "";
    }

    std::string GetProgramInfoLog(uint program) {
        // Get length of log messages
        GLint msgLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgLength);
        if (msgLength > 0) {
            // Get log message
            std::string log;
            log.resize(msgLength);
            glGetProgramInfoLog(program, msgLength, &msgLength, log.data());
            // Remove any trailing whitespace
            return log.substr(0, log.find_last_not_of("\t\r\n\f\v") + 1);
        }
        return "";
    }

    OpenGLShader::OpenGLShader(const Path& filepath)
        : OpenGLShader(filepath.stem().string(), filepath)
    {}

    OpenGLShader::OpenGLShader(const std::string& name, const Path& filepath)
        : m_Name(name)
    {
        VANTA_PROFILE_RENDER_FUNCTION();

        auto file = IO::File(filepath);
        SourceMap sources = SplitSource(file.Read());
        Compile(sources);
    }

    OpenGLShader::~OpenGLShader() {
        VANTA_PROFILE_RENDER_FUNCTION();
        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind() const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const {
        VANTA_PROFILE_RENDER_FUNCTION();
        glUseProgram(0);
    }

    void OpenGLShader::Compile(SourceMap sources) {
        VANTA_PROFILE_RENDER_FUNCTION();

        constexpr uint MAX_SHADERS = 2;

        VANTA_ASSERT(sources.size() <= MAX_SHADERS, "Only 2 shaders are supported!");

        GLuint program = glCreateProgram();

        std::array<GLenum, MAX_SHADERS> shaderIDs = {};

        // Compile shaders
        usize shaderIdx = 0;
        for (auto& src : sources) {
            GLenum type = src.first;
            const GLchar* source = (const GLchar*)src.second.c_str();

            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, 0);
            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                VANTA_ASSERT(false, "Failed to compile shader; {}", GetShaderInfoLog(shader));

                // Clean up
                glDeleteShader(shader);
                break;
            }

            glAttachShader(program, shader);
            shaderIDs[shaderIdx++] = shader;
        }

        // Link our program
        glLinkProgram(program);

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
            VANTA_ASSERT(false, "Failed to link shader; {}", GetProgramInfoLog(program));

            // Clean up
            glDeleteProgram(program);
            for (usize idx = 0; idx < shaderIdx; idx++) {
                glDeleteShader(shaderIDs[idx]);
            }

            return;
        }

        auto log = GetProgramInfoLog(program);
        if (!log.empty())
            VANTA_CORE_INFO(log);

        // Delete shaders; they're now part of the program
        for (usize idx = 0; idx < shaderIdx; idx++) {
            glDetachShader(program, shaderIDs[idx]);
            glDeleteShader(shaderIDs[idx]);
        }

        m_RendererID = program;
    }

    OpenGLShader::SourceMap OpenGLShader::SplitSource(const std::string& source) {
        VANTA_PROFILE_RENDER_FUNCTION();

        const char* delimToken = "#type";

        SourceMap sources;

        usize pos = source.find(delimToken, 0);
        while (pos != std::string::npos) {
            // Find the end of the line
            usize eol = source.find_first_of("\r\n", pos);

            // Get the shader type
            usize begin = pos + strlen(delimToken) + 1;
            std::string typeStr = source.substr(begin, eol - begin);
            Util::Trim(typeStr);

            auto type = StringToShaderType(typeStr);
            VANTA_ASSERT(type != 0, "Invalid shader type: '{}'", typeStr);

            // Get the next line
            usize nextLine = source.find_first_not_of("\r\n", eol);
            
            // Find the next delimiter
            pos = source.find(delimToken, nextLine);

            // Save shader source
            sources[type] = source.substr(nextLine, pos - nextLine);
        }

        return sources;
    }

    uint OpenGLShader::StringToShaderType(const std::string& str) {
        if (str == "vertex")                          { return GL_VERTEX_SHADER; }
        else if (str == "fragment" || str == "pixel") { return GL_FRAGMENT_SHADER; }
        else                                          { return 0; }
    }

    void OpenGLShader::SetInt(const std::string& name, int value) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniform1i(m_RendererID, loc, value);
    }

    void OpenGLShader::SetUInt(const std::string& name, uint value) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniform1ui(m_RendererID, loc, value);
    }

    void OpenGLShader::SetIntArray(const std::string& name, int* values, uint count) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniform1iv(m_RendererID, loc, count, values);
    }

    void OpenGLShader::SetUIntArray(const std::string& name, uint* values, uint count) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniform1uiv(m_RendererID, loc, count, values);
    }

    void OpenGLShader::SetFloat(const std::string& name, float value) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniform1f(m_RendererID, loc, value);
    }

    void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& values) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniform2f(m_RendererID, loc, values.x, values.y);
    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& values) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniform3f(m_RendererID, loc, values.x, values.y, values.z);
    }

    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& values) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniform4f(m_RendererID, loc, values.x, values.y, values.z, values.w);
    }

    void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& matrix) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniformMatrix3fv(m_RendererID, loc, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix) {
        VANTA_PROFILE_RENDER_FUNCTION();
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniformMatrix4fv(m_RendererID, loc, 1, GL_FALSE, glm::value_ptr(matrix));
    }
}
