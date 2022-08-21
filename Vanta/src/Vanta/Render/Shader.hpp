#pragma once

namespace Vanta {

    class Shader {
    public:
        class DataType {
        public:
            enum Type {
                None = 0,
                Int, Int2, Int3, Int4,
                UInt, UInt2, UInt3, UInt4,
                Float, Float2, Float3, Float4,
                Mat3, Mat4,
                Bool
            };

            DataType(Type type) : m_Type(type) {}
            virtual ~DataType() = default;

            uint ItemCount() const;
            uint Size() const;

            operator Type() const { return m_Type; }

        private:
            Type m_Type;
        };

        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual const std::string& GetName() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetUInt(const std::string& name, uint value) = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint count) = 0;
        virtual void SetUIntArray(const std::string& name, uint* values, uint count) = 0;

        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetFloat2(const std::string& name, const glm::vec2& values) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& values) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& values) = 0;

        virtual void SetMat3(const std::string& name, const glm::mat3& matrix) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& matrix) = 0;

        static Ref<Shader> Create(const Path& path);
        static Ref<Shader> Create(const std::string& name, const Path& filepath);

    protected:
        Shader() = default;
    };

    class ShaderLibrary {
    public:
        ShaderLibrary() = default;
        ~ShaderLibrary() = default;

        void Add(const Ref<Shader>& shader);
        Ref<Shader> Load(const std::string& filepath);
        Ref<Shader> Load(const std::string& name, const std::string& filepath);

        Ref<Shader> Get(const std::string& name);

        bool Exists(const std::string& name);

    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
}
