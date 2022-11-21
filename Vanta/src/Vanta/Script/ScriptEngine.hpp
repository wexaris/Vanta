#pragma once
#include "Vanta/Scene/Entity.hpp"

extern "C" {
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
}

namespace Vanta {

    class Scene;
    class ScriptClass;

    class ScriptInstance {
    public:
        ScriptInstance(Ref<ScriptClass> klass, Entity entity);

        void OnCreate();
        void OnUpdate(float delta);
        void OnDestroy();

    private:
        Ref<ScriptClass> m_ScriptClass;

        MonoObject* m_Instance = nullptr;
        MonoMethod* m_Constructor = nullptr;
        MonoMethod* m_OnCreateMethod = nullptr;
        MonoMethod* m_OnUpdateMethod = nullptr;
        MonoMethod* m_OnDestroyMethod = nullptr;
    };

    class ScriptClass {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& namespaceName, const std::string& className);

        MonoObject* Instantiate() const;
        MonoMethod* GetMethod(const std::string& name, int paramCount) const;
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr) const;

    private:
        MonoClass* m_Class = nullptr;

        std::string m_NamespaceName;
        std::string m_ClassName;
    };

    class ScriptEngine {
    public:
        static void Init();
        static void Shutdown();

        static void LoadAssembly(const Path& filepath);

        static void RuntimeBegin(Scene* scene);
        static void RuntimeEnd();

        static Ref<ScriptInstance> CreateInstance(const std::string& fullName, Entity entity);
        static bool EntityClassExists(const std::string& fullName);

        static Scene* GetContext();

    private:
        friend class ScriptClass;

        ScriptEngine() = delete;

        static void InitMono();
        static void ShutdownMono();

        static void InspectAssembly(MonoAssembly* assembly);
        static MonoObject* InstantiateClass(MonoClass* klass);
    };
}
