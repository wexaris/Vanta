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

    class ScriptClass {
    public:
        struct Field {
            std::string Name;
            FieldType Type;
            MonoClassField* MonoField;
        };

        ScriptClass() = default;
        ScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className);

        MonoObject* Instantiate() const;
        MonoMethod* GetMethod(const std::string& name, int paramCount) const;
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr) const;

        const std::unordered_map<std::string, Field>& GetFields() const { return m_Fields; }

        operator MonoClass* () { return m_Class; }

    private:
        friend class ScriptEngine;

        MonoClass* m_Class = nullptr;

        std::unordered_map<std::string, Field> m_Fields;

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

        static Ref<ScriptInstance> CreateInstance(const std::string& fullName, Entity entity);
        static bool EntityClassExists(const std::string& fullName);

        static Scene* GetContext();
        static MonoImage* GetCoreAssemblyImage();

    private:
        friend class ScriptClass;
        friend struct Interface;

        ScriptEngine() = delete;

        static void InitMono();
        static void ShutdownMono();

        static void InspectAssemblyImage(MonoImage* image);
        static MonoObject* InstantiateClass(MonoClass* klass);
    };
}
