#include "vantapch.hpp"
#include "Platform/OpenGL/Shader.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Util/String.hpp"

#include <glad/glad.h>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Vanta {

    namespace detail {
        static std::string GetProgramInfoLog(uint program) {
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

        static std::string GetShaderInfoLog(uint shader) {
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

        static Path GetCacheDirectory() {
            return Engine::RuntimeCacheDirectory() / "Shaders" / "OpenGL";
        }

        static void CreateCacheDirectory() {
            auto path = GetCacheDirectory();
            if (!std::filesystem::exists(path))
                std::filesystem::create_directories(path);
        }

        /// <summary>
        /// Find the corresponding OpenGL shader type for the given type string.
        /// </summary>
        /// <returns>
        /// Returns a non-zero value, if the string matches a type.
        /// Returns a zero, if the string doesn't match any type. Doesn't log an error.
        /// </returns>
        static uint StringToShaderType(const std::string& str) {
            if (str == "vertex")                          { return GL_VERTEX_SHADER; }
            else if (str == "fragment" || str == "pixel") { return GL_FRAGMENT_SHADER; }
            else                                          { return 0; }
        }

        static const char* ShaderTypeToString(GLenum type) {
            switch (type) {
            case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
            case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
            }
            VANTA_UNREACHABLE("Invalid shader type!");
            return nullptr;
        }

        static shaderc_shader_kind ShaderTypeToShaderC(GLenum type) {
            switch (type) {
            case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
            case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
            }
            VANTA_UNREACHABLE("Invalid shader type!");
            return (shaderc_shader_kind)0;
        }

        static const char* ShaderTypeCachedFileExtensionOpenGL(GLenum type) {
            switch (type) {
            case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
            case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
            }
            VANTA_UNREACHABLE("Invalid shader type!");
            return "";
        }

        static const char* ShaderTypeCachedFileExtensionVulkan(GLenum type) {
            switch (type) {
            case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
            case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
            }
            VANTA_UNREACHABLE("Invalid shader type!");
            return "";
        }
    }

    OpenGLShader::OpenGLShader(const Path& filepath)
        : OpenGLShader(filepath.stem().string(), filepath)
    {}

    OpenGLShader::OpenGLShader(const std::string& name, const Path& filepath)
        : m_Name(name), m_Filepath(filepath)
    {
        VANTA_PROFILE_RENDER_FUNCTION();

        detail::CreateCacheDirectory();

        std::string source = IO::File(filepath).Read();
        auto sources = PreProcess(source);

        constexpr bool spirv = false;
        if constexpr (spirv) {
            VANTA_PROFILE_RENDER_SCOPE("Shader Creation");
            CompileOrLoadVulkanBinaries(sources);
            CompileOrLoadOpenGLBinaries();
            CreateProgram();
        }
        else {
            VANTA_PROFILE_RENDER_SCOPE("Shader Creation");
            CompileOpenGLSources(sources);
        }
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

    std::unordered_map<uint, std::string> OpenGLShader::PreProcess(const std::string& source) {
        VANTA_PROFILE_RENDER_FUNCTION();

        const char* delimToken = "#type";

        std::unordered_map<uint, std::string> sources;

        usize pos = source.find(delimToken, 0);
        while (pos != std::string::npos) {
            // Find the end of the line
            usize eol = source.find_first_of("\r\n", pos);

            // Get the shader type
            usize begin = pos + strlen(delimToken) + 1;
            std::string typeStr = source.substr(begin, eol - begin);
            String::Trim(typeStr);

            auto type = detail::StringToShaderType(typeStr);
            VANTA_CORE_ASSERT(type != 0, "Invalid shader type: '{}'", typeStr);

            // Get the next line
            usize nextLine = source.find_first_not_of("\r\n", eol);
            
            // Find the next delimiter
            pos = source.find(delimToken, nextLine);

            // Save shader source
            sources[type] = source.substr(nextLine, pos - nextLine);
        }

        return sources;
    }

    void OpenGLShader::CompileOrLoadVulkanBinaries(const std::unordered_map<uint, std::string>& sources) {
        VANTA_PROFILE_RENDER_FUNCTION();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        constexpr bool optimize = true;
        if constexpr (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        auto cacheDir = detail::GetCacheDirectory();

        m_VulkanSPIRV.clear();
        for (auto&& [type, source] : sources) {
            std::filesystem::path cachedPath = cacheDir / (m_Filepath.filename().string() + detail::ShaderTypeCachedFileExtensionVulkan(type));

            auto cacheFile = IO::File(cachedPath);

            if (cacheFile.Exists()) {
                ScopedBuffer buffer = cacheFile.ReadBytes();
                uint32* beg = buffer.As<uint32>();
                usize size = buffer.Size() / sizeof(uint32);
                m_VulkanSPIRV[type] = std::vector<uint32>(beg, beg + size);
            }
            else {
                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, detail::ShaderTypeToShaderC(type), m_Filepath.string().c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                    VANTA_CORE_ERROR("Failed to compile shader: {} - {}", m_Filepath, detail::ShaderTypeToString(type));
                    VANTA_CORE_ERROR("{}", module.GetErrorMessage());
                    return;
                }

                m_VulkanSPIRV[type] = std::vector<uint32>(module.cbegin(), module.cend());

                auto& data = m_VulkanSPIRV[type];
                cacheFile.Write((char*)data.data(), data.size() * sizeof(uint32));
            }
        }

        for (auto&& [type, data] : m_VulkanSPIRV)
            Reflect(type, data);
    }

    void OpenGLShader::CompileOrLoadOpenGLBinaries() {
        VANTA_PROFILE_RENDER_FUNCTION();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        constexpr bool optimize = true;
        if constexpr (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        auto cacheDir = detail::GetCacheDirectory();

        m_OpenGLSources.clear();
        m_OpenGLSPIRV.clear();
        for (auto&& [type, spirv] : m_VulkanSPIRV) {
            std::filesystem::path cachedPath = cacheDir / (m_Filepath.filename().string() + detail::ShaderTypeCachedFileExtensionOpenGL(type));

            auto cacheFile = IO::File(cachedPath);

            if (cacheFile.Exists()) {
                ScopedBuffer buffer = cacheFile.ReadBytes();
                uint32* beg = buffer.As<uint32>();
                usize size = buffer.Size() / sizeof(uint32);
                m_OpenGLSPIRV[type] = std::vector<uint32>(beg, beg + size);
            }
            else {
                spirv_cross::CompilerGLSL glslCompiler(spirv);
                m_OpenGLSources[type] = glslCompiler.compile();

                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(m_OpenGLSources[type], detail::ShaderTypeToShaderC(type), m_Name.c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                    VANTA_CORE_ERROR("Failed to compile shader: {} - {}", m_Filepath, detail::ShaderTypeToString(type));
                    VANTA_CORE_ERROR("{}", module.GetErrorMessage());
                    return;
                }

                m_OpenGLSPIRV[type] = std::vector<uint32>(module.cbegin(), module.cend());

                auto& data = m_OpenGLSPIRV[type];
                cacheFile.Write((char*)data.data(), data.size() * sizeof(uint32));
            }
        }
    }

    void OpenGLShader::CreateProgram() {
        VANTA_PROFILE_RENDER_FUNCTION();

        GLuint program = glCreateProgram();

        std::vector<GLuint> shaderIDs;
        for (auto&& [type, spirv] : m_OpenGLSPIRV) {
            GLuint shaderID = glCreateShader(type);
            glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), (GLsizei)(spirv.size() * sizeof(uint32)));
            glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);

            GLint good = 0;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &good);
            if (good == GL_FALSE) {
                VANTA_CORE_ERROR("Failed to compile shader: {} - {}\n{}", m_Filepath, detail::ShaderTypeToString(type), detail::GetShaderInfoLog(shaderID));

                glDeleteShader(shaderID);
                glDeleteProgram(program);
                return;
            }

            glAttachShader(program, shaderID);
            shaderIDs.push_back(shaderID);
        }

        glLinkProgram(program);

        GLint good;
        glGetProgramiv(program, GL_LINK_STATUS, &good);
        if (good == GL_FALSE) {
            VANTA_CORE_ERROR("Failed to link shader: {}\n{}", m_Filepath, detail::GetProgramInfoLog(program));

            for (auto id : shaderIDs) {
                glDetachShader(program, id);
                glDeleteShader(id);
            }
            glDeleteProgram(program);
            return;
        }

        for (auto shaderID : shaderIDs) {
            glDetachShader(program, shaderID);
            glDeleteShader(shaderID);
        }

        m_RendererID = program;
    }

    void OpenGLShader::CompileOpenGLSources(const std::unordered_map<uint, std::string>& sources) {
        VANTA_PROFILE_RENDER_FUNCTION();

        GLuint program = glCreateProgram();

        std::vector<GLuint> shaderIDs;
        for (auto&& [type, source] : sources) {
            GLuint shaderID = glCreateShader(type);

            const GLchar* sourceStr = source.c_str();
            glShaderSource(shaderID, 1, &sourceStr, 0);
            glCompileShader(shaderID);

            GLint good = 0;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &good);
            if (good == GL_FALSE) {
                VANTA_CORE_ERROR("Failed to compile shader: {} - {}\n{}", m_Filepath, detail::ShaderTypeToString(type), detail::GetShaderInfoLog(shaderID));

                glDeleteShader(shaderID);
                glDeleteProgram(program);
                return;
            }

            glAttachShader(program, shaderID);
            shaderIDs.push_back(shaderID);
        }

        glLinkProgram(program);

        GLint good;
        glGetProgramiv(program, GL_LINK_STATUS, &good);
        if (good == GL_FALSE) {
            VANTA_CORE_ERROR("Failed to link shader: {}\n{}", m_Filepath, detail::GetProgramInfoLog(program));

            for (auto id : shaderIDs) {
                glDetachShader(program, id);
                glDeleteShader(id);
            }
            glDeleteProgram(program);
            return;
        }

        for (auto id : shaderIDs) {
            glDetachShader(program, id);
            glDeleteShader(id);
        }

        m_RendererID = program;
    }

    void OpenGLShader::Reflect(uint type, const std::vector<uint32>& data) {
        spirv_cross::Compiler compiler(data.data(), data.size());
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        VANTA_CORE_TRACE("OpenGLShader::Reflect - {} {}", detail::ShaderTypeToString(type), m_Filepath);
        VANTA_CORE_TRACE("    {} uniform buffers", resources.uniform_buffers.size());
        VANTA_CORE_TRACE("    {} resources", resources.sampled_images.size());

        if (resources.uniform_buffers.size() > 0) {
            VANTA_CORE_TRACE("Uniform buffers:");
            for (const auto& resource : resources.uniform_buffers) {
                const auto& bufferType = compiler.get_type(resource.base_type_id);
                usize bufferSize = compiler.get_declared_struct_size(bufferType);
                uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                usize memberCount = bufferType.member_types.size();

                VANTA_CORE_TRACE("  {}", resource.name);
                VANTA_CORE_TRACE("    Size = {}", bufferSize);
                VANTA_CORE_TRACE("    Binding = {}", binding);
                VANTA_CORE_TRACE("    Members = {}", memberCount);
            }
        }
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
