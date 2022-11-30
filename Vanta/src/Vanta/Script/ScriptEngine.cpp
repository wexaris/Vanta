#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Scene/Scene.hpp"
#include "Vanta/Script/Interface.hpp"
#include "Vanta/Script/ScriptEngine.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/tabledefs.h>

#ifdef VANTA_DEBUG
    #include <mono/metadata/mono-debug.h>
#endif

namespace Vanta {

    namespace detail {
        static MonoAssembly* LoadMonoAssembly(const Path& filepath) {
            ScopedBuffer data = IO::File(filepath).ReadBytes();
            if (!data) {
                VANTA_CORE_CRITICAL("Failed to read C# assembly: {}", filepath);
                return nullptr;
            }

            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(data.As<char>(), (uint32)data.Size(), 1, &status, 0);

            if (status != MONO_IMAGE_OK) {
                const char* error = mono_image_strerror(status);
                VANTA_CORE_CRITICAL("Failed to load C# assembly image: {}", error);
                return nullptr;
            }

            std::string pathStr = filepath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);

#ifdef VANTA_DEBUG // Load PDB
            Path pdbFilepath = filepath;
            pdbFilepath.replace_extension(".pdb");

            IO::File pdbFile(pdbFilepath);
            if (pdbFile.Exists()) {
                ScopedBuffer pdbData = pdbFile.ReadBytes();
                if (pdbData) {
                    mono_debug_open_image_from_memory(image, pdbData.As<const mono_byte>(), (int)pdbData.Size());
                    VANTA_CORE_DEBUG("Loaded script PDB: {}", pdbFilepath);
                }
            }
#endif

            mono_image_close(image);

            return assembly;
        }

        static void PrintAssemblyClasses(MonoAssembly* assembly) {
            MonoImage* image = mono_assembly_get_image(assembly);
            const MonoTableInfo* typeDefs = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
            int32 numClasses = mono_table_info_get_rows(typeDefs);

            for (int32 i = 0; i < numClasses; i++) {
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

        // Runtime
        Scene* SceneContext = nullptr;

        // Editor
        Path CoreAssemblyFilepath;

        Box<IO::FileWatcher> AppAssemblyFileWatcher;
        bool AppAssemblyReloadPending = false;

        std::unordered_map<UUID, std::unordered_map<std::string, Box<ScriptFieldInstance>>> EntityFieldInstances;
    };

    static ScriptData s_Data;

    void ScriptEngine::Init() {
        VANTA_PROFILE_FUNCTION();

        InitMono();

        Interface::RegisterFunctions();

        Path coreAssemblyPath = Engine::RuntimeResourceDirectory() / "Scripts" / "Vanta-Scripts.dll";
        if (!LoadCoreAssembly(coreAssemblyPath)) {
            VANTA_CORE_CRITICAL("Failed to load script core assembly!");
            return;
        }

        Interface::RegisterComponents();
    }

    void ScriptEngine::Shutdown() {
        VANTA_PROFILE_FUNCTION();

        ShutdownMono();
    }

    void ScriptEngine::InitMono() {
        VANTA_PROFILE_FUNCTION();

        mono_set_assemblies_path("mono/4.5");

#ifdef VANTA_DEBUG
        const char* argv[] = {
            "--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebug.log",
            "--soft-breakpoints"
        };

        mono_jit_parse_options(2, (char**)argv);
        mono_debug_init(MONO_DEBUG_FORMAT_MONO);
#endif

        s_Data.RootDomain = mono_jit_init("VantaJIT");
        VANTA_CORE_ASSERT(s_Data.RootDomain, "Failed to initialize Mono JIT runtime!");

#ifdef VANTA_DEBUG
        mono_debug_domain_create(s_Data.RootDomain);
#endif
    }

    void ScriptEngine::ShutdownMono() {
        VANTA_PROFILE_FUNCTION();

        mono_domain_set(mono_get_root_domain(), false);

        mono_domain_unload(s_Data.AppDomain);
        s_Data.AppDomain = nullptr;

        mono_jit_cleanup(s_Data.RootDomain);
        s_Data.RootDomain = nullptr;
    }

    bool ScriptEngine::LoadCoreAssembly(const Path& filepath) {
        VANTA_PROFILE_FUNCTION();

        // Create new app domain
        std::string domainName = "VantaScripts";
        s_Data.AppDomain = mono_domain_create_appdomain(domainName.data(), nullptr);
        mono_domain_set(s_Data.AppDomain, true);

        // Load assembly
        s_Data.CoreAssemblyFilepath = filepath;
        s_Data.CoreAssembly = detail::LoadMonoAssembly(filepath);
        if (!s_Data.CoreAssembly)
            return false;

        s_Data.CoreAssemblyImage = mono_assembly_get_image(s_Data.CoreAssembly);

        // Save entity class
        s_Data.EntityClass = ScriptClass(s_Data.CoreAssemblyImage, "Vanta", "Entity");

        return true;
    }

    static void OnAppAssemblyFileChange(const std::string&, const filewatch::Event type) {
        if (!s_Data.AppAssemblyReloadPending && type == filewatch::Event::modified) {
            s_Data.AppAssemblyReloadPending = true;

            // Queue app assembly reload with main thread
            Engine::Get().SubmitToMainThread([]() {
                ScriptEngine::ReloadAssembly();
            });
        }
    }

    bool ScriptEngine::LoadAppAssembly(const Path& filepath) {
        VANTA_PROFILE_FUNCTION();

        // Remove file watcher
        s_Data.AppAssemblyFileWatcher.reset();

        // Load assembly
        s_Data.AppAssembly = detail::LoadMonoAssembly(filepath);
        if (!s_Data.AppAssembly)
            return false;

        s_Data.AppAssemblyImage = mono_assembly_get_image(s_Data.AppAssembly);

        // Get needed data from assembly
        InspectAssemblyImage(s_Data.AppAssemblyImage);

        // Attach file watcher
        s_Data.AppAssemblyFileWatcher = NewBox<IO::FileWatcher>(filepath.string(), OnAppAssemblyFileChange);
        s_Data.AppAssemblyReloadPending = false;

        return true;
    }

    void ScriptEngine::ReloadAssembly() {
        VANTA_PROFILE_FUNCTION();

        // Unload current app domain
        mono_domain_set(mono_get_root_domain(), false);
        mono_domain_unload(s_Data.AppDomain);
        s_Data.AppDomain = nullptr;

        // Load assemblies
        if (!LoadCoreAssembly(s_Data.CoreAssemblyFilepath)) {
            VANTA_CORE_CRITICAL("Failed to load script core assembly!");
            return;
        }
        Path appAssemblyPath = Project::GetRootDirectory() / Project::GetActive()->GetConfig().ScriptAssemblyPath;
        if (!LoadAppAssembly(appAssemblyPath)) {
            VANTA_CORE_CRITICAL("Failed to load app script assembly!");
            return;
        }

        // Re-register components in new domain
        Interface::RegisterComponents();
    }

    void ScriptEngine::InspectAssemblyImage(MonoImage* image) {
        VANTA_PROFILE_FUNCTION();

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
        VANTA_PROFILE_FUNCTION();

        s_Data.SceneContext = context;
    }

    void ScriptEngine::RuntimeEnd() {
        VANTA_PROFILE_FUNCTION();

        s_Data.SceneContext = nullptr;
    }

    Ref<ScriptInstance> ScriptEngine::Instantiate(std::string fullName, Entity entity) {
        VANTA_PROFILE_FUNCTION();
        VANTA_CORE_ASSERT(ClassExists(fullName), "Invalid class!");
        VANTA_CORE_ASSERT(entity, "Invalid entity!");

        Ref<ScriptInstance> instance = NewRef<ScriptInstance>(GetClass(fullName), entity);

        // Set variables modified in editor
        UUID entityId = entity.GetUUID();
        for (auto& [name, field] : s_Data.EntityFieldInstances[entityId]) {
            instance->SetFieldValue_Impl(name, field->m_Buffer);
        }

        return instance;
    }

    MonoObject* ScriptEngine::CreateObject(MonoClass* klass) {
        VANTA_CORE_ASSERT(klass, "Invalid class!");
        return mono_object_new(s_Data.AppDomain, klass);
    }

    bool ScriptEngine::ClassExists(const std::string& fullName) {
        return s_Data.EntityClasses.contains(fullName);
    }

    Ref<ScriptClass> ScriptEngine::GetClass(const std::string& fullName) {
        VANTA_CORE_ASSERT(ClassExists(fullName), "Invalid class!");
        return s_Data.EntityClasses.at(fullName);
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

    void ScriptEngine::ClearFieldInstances() {
        s_Data.EntityFieldInstances.clear();
    }
}
