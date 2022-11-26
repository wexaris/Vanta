#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Scene/Scene.hpp"
#include "Vanta/Script/Interface.hpp"
#include "Vanta/Script/ScriptEngine.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/tabledefs.h>

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

        static void PrintAssemblyTypes(MonoAssembly* assembly) {
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

        static std::unordered_map<std::string, ScriptFieldType> s_MonoFieldTypeMap = {
            { "System.Boolean", ScriptFieldType::Bool },
            { "System.Char", ScriptFieldType::Char },

            { "System.Byte", ScriptFieldType::Int8 },
            { "System.Int16", ScriptFieldType::Int16 },
            { "System.Int32", ScriptFieldType::Int32 },
            { "System.Int64", ScriptFieldType::Int64 },

            { "System.UByte", ScriptFieldType::UInt8 },
            { "System.UInt16", ScriptFieldType::UInt16 },
            { "System.UInt32", ScriptFieldType::UInt32 },
            { "System.UInt64", ScriptFieldType::UInt64 },

            { "System.Single", ScriptFieldType::Float },
            { "System.Double", ScriptFieldType::Double },

            { "Vanta.Vector2", ScriptFieldType::Vector2 },
            { "Vanta.Vector3", ScriptFieldType::Vector3 },
            { "Vanta.Vector4", ScriptFieldType::Vector4 },

            { "Vanta.Entity", ScriptFieldType::Entity },
        };

        static ScriptFieldType MonoTypeToFieldType(MonoType* type) {
            std::string name = mono_type_get_name(type);
            auto it = s_MonoFieldTypeMap.find(name);
            if (it == s_MonoFieldTypeMap.end()) {
                VANTA_CORE_ERROR("Invalid script field type: {}", name);
                return ScriptFieldType::None;
            }
            return it->second;
        }
    }

    struct ScriptData {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;

        MonoAssembly* CoreAssembly = nullptr;
        MonoImage* CoreAssemblyImage = nullptr;

        MonoAssembly* AppAssembly = nullptr;
        MonoImage* AppAssemblyImage = nullptr;

        ScriptClass EntityClass;
        std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;

        // Editor
        std::unordered_map<UUID, std::unordered_map<std::string, Box<ScriptFieldInstance>>> EntityFieldInstances;

        // Runtime
        Scene* SceneContext = nullptr;
    };

    static ScriptData s_Data;

    void ScriptEngine::Init() {
        InitMono();

        LoadCoreAssembly(Engine::Get().ScriptDirectory() / "Vanta-Script.dll");
        s_Data.EntityClass = ScriptClass(s_Data.CoreAssemblyImage, "Vanta", "Entity");

        LoadAppAssembly(Engine::Get().ScriptDirectory() / "Sandbox-Script.dll");
        InspectAssemblyImage(s_Data.AppAssemblyImage);

        Interface::RegisterFunctions();
        Interface::RegisterComponents();
    }

    void ScriptEngine::Shutdown() {
        ShutdownMono();
    }

    void ScriptEngine::InitMono() {
        mono_set_assemblies_path("mono/4.5");

        s_Data.RootDomain = mono_jit_init("VantaJIT");
        VANTA_CORE_ASSERT(s_Data.RootDomain, "Failed to initialize Mono JIT runtime!");

        std::string domainName = "VantaScripts";
        s_Data.AppDomain = mono_domain_create_appdomain(domainName.data(), nullptr);
        mono_domain_set(s_Data.AppDomain, true);
    }

    void ScriptEngine::ShutdownMono() {
        mono_jit_cleanup(s_Data.RootDomain);
        s_Data.AppDomain = nullptr;
        s_Data.RootDomain = nullptr;
    }

    void ScriptEngine::LoadCoreAssembly(const Path& filepath) {
        s_Data.CoreAssembly = detail::LoadMonoAssembly(filepath);
        s_Data.CoreAssemblyImage = mono_assembly_get_image(s_Data.CoreAssembly);
    }

    void ScriptEngine::LoadAppAssembly(const Path& filepath) {
        s_Data.AppAssembly = detail::LoadMonoAssembly(filepath);
        s_Data.AppAssemblyImage = mono_assembly_get_image(s_Data.AppAssembly);
    }

    void ScriptEngine::InspectAssemblyImage(MonoImage* image) {
        s_Data.EntityClasses.clear();

        const MonoTableInfo* typeDefs = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32 typeCount = mono_table_info_get_rows(typeDefs);

        for (int32 i = 0; i < typeCount; i++) {
            uint32 cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefs, i, cols, MONO_TYPEDEF_SIZE);

            const char* namespaceName = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* className = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            MonoClass* klass = mono_class_from_name(image, namespaceName, className);

            if (klass == s_Data.EntityClass)
                continue;

            bool isEntity = mono_class_is_subclass_of(klass, (MonoClass*)s_Data.EntityClass, false);
            if (!isEntity)
                continue;

            // Save classes that derive our `Entity` class
            std::string fullName = (strlen(namespaceName) != 0) ? FMT("{}.{}", namespaceName, className) : className;
            Ref<ScriptClass> scriptClass = NewRef<ScriptClass>(image, namespaceName, className);
            s_Data.EntityClasses[fullName] = scriptClass;

            // Save public fields
            void* fieldIterator = nullptr;
            while (MonoClassField* field = mono_class_get_fields(klass, &fieldIterator)) {
                auto flags = mono_field_get_flags(field);

                bool isPublic = flags & FIELD_ATTRIBUTE_PUBLIC;
                if (!isPublic)
                    continue;

                const char* name = mono_field_get_name(field);
                ScriptFieldType type = detail::MonoTypeToFieldType(mono_field_get_type(field));

                scriptClass->m_Fields[name] = { name, type, field };
            }
        }
    }

    void ScriptEngine::RuntimeBegin(Scene* context) {
        s_Data.SceneContext = context;
    }

    void ScriptEngine::RuntimeEnd() {
        s_Data.SceneContext = nullptr;
    }

    Ref<ScriptInstance> ScriptEngine::Instantiate(std::string fullName, Entity entity) {
        VANTA_CORE_ASSERT(ClassExists(fullName), "Invalid class!");
        VANTA_CORE_ASSERT(entity, "Invalid entity!");

        Ref<ScriptInstance> instance = NewRef<ScriptInstance>(s_Data.EntityClasses[fullName], entity);

        UUID entityId = entity.GetUUID();
        for (auto& [name, field] : s_Data.EntityFieldInstances[entityId]) {
            instance->SetFieldValue_Impl(name, field->m_Buffer);
        }

        return instance;
    }

    bool ScriptEngine::ClassExists(const std::string& fullName) {
        return s_Data.EntityClasses.contains(fullName);
    }

    Ref<ScriptClass> ScriptEngine::GetClass(const std::string& fullName) {
        VANTA_CORE_ASSERT(ClassExists(fullName), "Invalid class!");
        return s_Data.EntityClasses[fullName];
    }

    Ref<ScriptClass> ScriptEngine::TryGetClass(const std::string& fullName) {
        return s_Data.EntityClasses[fullName];
    }

    const ScriptClass& ScriptEngine::GetEntityClass() {
        return s_Data.EntityClass;
    }

    Scene* ScriptEngine::GetContext() {
        return s_Data.SceneContext;
    }

    MonoImage* ScriptEngine::GetCoreAssemblyImage() {
        return s_Data.CoreAssemblyImage;
    }

    std::unordered_map<std::string, Box<ScriptFieldInstance>>& ScriptEngine::GetFieldInstances(Entity entity) {
        VANTA_CORE_ASSERT(entity, "Invalid entity!");
        return s_Data.EntityFieldInstances[entity.GetUUID()];
    }

    MonoObject* ScriptEngine::CreateObject(MonoClass* klass) {
        VANTA_CORE_ASSERT(klass, "Invalid class!");
        return mono_object_new(s_Data.AppDomain, klass);
    }
}
