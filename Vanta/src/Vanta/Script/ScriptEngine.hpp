#pragma once
#include "Vanta/Scene/Entity.hpp"

extern "C" {
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoClassField MonoClassField;
}

namespace Vanta {

    class Scene;

    enum class FieldType {
        None = 0,
        Bool, Char,
        Int8, Int16, Int32, Int64,
        UInt8, UInt16, UInt32, UInt64,
        Float, Double,
        Vector2, Vector3, Vector4,
        Entity,
    };

    struct ScriptField {
        std::string Name;
        FieldType Type;
        MonoClassField* MonoField;
    };

    struct ScriptFieldInstance {
        ScriptField Field;
        
        template<typename T>
        T GetFieldValue() {
            return *(T*)m_Buffer;
        }

        template<typename T>
        void SetFieldValue(const T& value) {
            VANTA_CORE_ASSERT(sizeof(T) <= m_Size, "Type too large for script field!");
            memcpy(m_Buffer, &value, sizeof(T));
        }

    protected:
        ScriptFieldInstance(ScriptField field, void* data, usize size)
            : Field(field), m_Buffer(data), m_Size(size)
        {
            memset(m_Buffer, 0, m_Size);
        }

    private:
        friend class ScriptEngine;

        void* m_Buffer = nullptr;
        const usize m_Size;
    };

    template<typename T>
    struct ScriptFieldBuffer : public ScriptFieldInstance {
        static constexpr usize SIZE = sizeof(T);
        char Buffer[SIZE];

        ScriptFieldBuffer(ScriptField field) : ScriptFieldInstance(field, Buffer, SIZE) {}

        ScriptFieldBuffer(ScriptField field, const T& value) : ScriptFieldInstance(field, Buffer, SIZE) {
            SetFieldValue(value);
        }
    };

    class ScriptClass {
    public:
        ScriptClass() = default;
        ScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className);

        MonoObject* Instantiate() const;
        MonoMethod* GetMethod(const std::string& name, int paramCount) const;
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr) const;

        const std::unordered_map<std::string, ScriptField>& GetFields() const { return m_Fields; }

        operator MonoClass* () { return m_Class; }

    private:
        friend class ScriptEngine;

        MonoClass* m_Class = nullptr;

        std::unordered_map<std::string, ScriptField> m_Fields;

        std::string m_NamespaceName;
        std::string m_ClassName;
    };

    class ScriptInstance {
    public:
        ScriptInstance(Ref<ScriptClass> klass, Entity entity);

        void OnCreate();
        void OnUpdate(float delta);
        void OnDestroy();

        Ref<ScriptClass>& GetClass() { return m_ScriptClass; }

        template<typename T>
        T GetFieldValue(const std::string& name) {
            static char buffer[sizeof(T)];
            bool ok = GetFieldValue_Impl(name, buffer);
            if (!ok)
                return T();
            return *(T*)buffer;
        }

        template<typename T>
        void SetFieldValue(const std::string& name, const T& value) {
            SetFieldValue_Impl(name, &value);
        }

    private:
        friend class ScriptEngine;

        Ref<ScriptClass> m_ScriptClass;

        MonoObject* m_Instance = nullptr;
        MonoMethod* m_Constructor = nullptr;
        MonoMethod* m_OnCreateMethod = nullptr;
        MonoMethod* m_OnUpdateMethod = nullptr;
        MonoMethod* m_OnDestroyMethod = nullptr;

        bool GetFieldValue_Impl(const std::string& name, void* buffer);
        bool SetFieldValue_Impl(const std::string& name, const void* data);
    };

    class ScriptEngine {
    public:
        static void Init();
        static void Shutdown();

        static void LoadCoreAssembly(const Path& filepath);
        static void LoadAppAssembly(const Path& filepath);

        static void RuntimeBegin(Scene* scene);
        static void RuntimeEnd();

        static Ref<ScriptInstance> Instantiate(std::string fullName, Entity entity);

        static bool ClassExists(const std::string& fullName);
        static Ref<ScriptClass>& GetClass(const std::string& fullName);

        static Scene* GetContext();
        static MonoImage* GetCoreAssemblyImage();

        static std::unordered_map<std::string, Box<ScriptFieldInstance>>& GetFieldInstances(Entity entity);

    private:
        friend class ScriptClass;
        friend struct Interface;

        ScriptEngine() = delete;

        static void InitMono();
        static void ShutdownMono();

        static void InspectAssemblyImage(MonoImage* image);
    };
}
