#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Scene/Scene.hpp"
#include "Vanta/Script/Interface.hpp"
#include "Vanta/Script/ScriptEngine.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Vanta {

    namespace detail {
        static MonoAssembly* LoadMonoAssembly(const Path& filepath) {
            auto data = IO::File(filepath).ReadBinary();

            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(data.data(), (uint32)data.size(), 1, &status, 0);

            if (status != MONO_IMAGE_OK) {
                const char* error = mono_image_strerror(status);
                VANTA_CORE_CRITICAL("Failed to load C# assembly: {}", error);
                return nullptr;
            }

            std::string pathStr = filepath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);
            mono_image_close(image);

            return assembly;
        }

        void PrintAssemblyTypes(MonoAssembly* assembly) {
            MonoImage* image = mono_assembly_get_image(assembly);
            const MonoTableInfo* typeDefs = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
            int32 numTypes = mono_table_info_get_rows(typeDefs);

            for (int32 i = 0; i < numTypes; i++) {
                uint32 cols[MONO_TYPEDEF_SIZE];
                mono_metadata_decode_row(typeDefs, i, cols, MONO_TYPEDEF_SIZE);

                const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
                const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

                VANTA_CORE_INFO("{}.{}", nameSpace, name);
            }
        }
    }

    struct ScriptData {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;

        MonoAssembly* CoreAssembly = nullptr;
        MonoImage* CoreAssemblyImage = nullptr;

        std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;

        Scene* SceneContext = nullptr;
    };

    static ScriptData s_Data;

    void ScriptEngine::Init() {
        InitMono();
        LoadAssembly(Engine::Get().ScriptDirectory() / "Vanta-Script.dll");

        Interface::RegisterFunctions();
    }

    void ScriptEngine::Shutdown() {
        ShutdownMono();
    }

    void ScriptEngine::InitMono() {
        mono_set_assemblies_path("mono/4.5");

        s_Data.RootDomain = mono_jit_init("VantaJIT");
        VANTA_CORE_ASSERT(s_Data.RootDomain, "Failed to initialize Mono JIT runtime!");

        s_Data.AppDomain = mono_domain_create_appdomain(const_cast<char*>("VantaScripts"), nullptr);
        mono_domain_set(s_Data.AppDomain, true);
    }

    void ScriptEngine::ShutdownMono() {
        mono_jit_cleanup(s_Data.RootDomain);
        s_Data.AppDomain = nullptr;
        s_Data.RootDomain = nullptr;
    }

    void ScriptEngine::LoadAssembly(const Path& filepath) {
        s_Data.CoreAssembly = detail::LoadMonoAssembly(filepath);
        s_Data.CoreAssemblyImage = mono_assembly_get_image(s_Data.CoreAssembly);
        InspectAssembly(s_Data.CoreAssembly);
    }

    void ScriptEngine::InspectAssembly(MonoAssembly* assembly) {
        s_Data.EntityClasses.clear();

        MonoImage* image = mono_assembly_get_image(assembly);
        const MonoTableInfo* typeDefs = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32 typeCount = mono_table_info_get_rows(typeDefs);

        MonoClass* entityClass = mono_class_from_name(s_Data.CoreAssemblyImage, "Vanta", "Entity");

        for (int32 i = 0; i < typeCount; i++) {
            uint32 cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefs, i, cols, MONO_TYPEDEF_SIZE);

            const char* namespaceName = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* className = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            MonoClass* klass = mono_class_from_name(image, namespaceName, className);

            if (klass == entityClass)
                continue;

            // Save classes that derive our `Entity` class
            bool isEntity = mono_class_is_subclass_of(klass, entityClass, false);

            if (isEntity) {
                std::string fullName = (strlen(namespaceName) != 0) ?
                    FMT("{}.{}", namespaceName, className) : className;

                s_Data.EntityClasses[fullName] = NewRef<ScriptClass>(namespaceName, className);
            }
        }
    }

    MonoObject* ScriptEngine::InstantiateClass(MonoClass* klass) {
        VANTA_CORE_ASSERT(klass, "Invalid script class!");
        MonoObject* object = mono_object_new(s_Data.AppDomain, klass);
        mono_runtime_object_init(object);
        return object;
    }

    void ScriptEngine::RuntimeBegin(Scene* scene) {
        s_Data.SceneContext = scene;
    }

    void ScriptEngine::RuntimeEnd() {
        s_Data.SceneContext = nullptr;
    }

    Ref<ScriptInstance> ScriptEngine::CreateInstance(const std::string& fullName) {
        VANTA_CORE_ASSERT(EntityClassExists(fullName), "Invalid class!");
        return NewRef<ScriptInstance>(s_Data.EntityClasses[fullName]);
    }

    bool ScriptEngine::EntityClassExists(const std::string& fullName) {
        return s_Data.EntityClasses.find(fullName) != s_Data.EntityClasses.end();
    }

    ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className)
        : m_NamespaceName(namespaceName), m_ClassName(className)
    {
        m_Class = mono_class_from_name(s_Data.CoreAssemblyImage, namespaceName.c_str(), className.c_str());
        if (!m_Class)
            VANTA_CORE_CRITICAL("Failed to retrieve class from C# assembly: {}.{}", namespaceName, className);
    }

    MonoObject* ScriptClass::Instantiate() const {
        return ScriptEngine::InstantiateClass(m_Class);
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int paramCount) const {
        MonoMethod* method = mono_class_get_method_from_name(m_Class, name.c_str(), paramCount);
        if (!method)
            VANTA_CORE_CRITICAL("Failed to retrieve class method: {}({})", name, paramCount);

        return method;
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params) const {
        VANTA_CORE_ASSERT(instance, "Invalid script class instance!");
        VANTA_CORE_ASSERT(method, "Invalid script class method!");
        return mono_runtime_invoke(method, instance, params, nullptr);
    }

    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass) : m_ScriptClass(scriptClass)
    {
        m_Instance = m_ScriptClass->Instantiate();
        m_OnCreateMethod = m_ScriptClass->GetMethod("OnCreate", 0);
        m_OnUpdateMethod = m_ScriptClass->GetMethod("OnUpdate", 1);
        m_OnDestroyMethod = m_ScriptClass->GetMethod("OnDestroy", 0);
    }

    void ScriptInstance::OnCreate() {
        m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
    }

    void ScriptInstance::OnUpdate(float delta) {
        void* param = &delta;
        m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
    }

    void ScriptInstance::OnDestroy() {
        m_ScriptClass->InvokeMethod(m_Instance, m_OnDestroyMethod);
    }
}
