#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Scene/Scene.hpp"
#include "Vanta/Scripts/Instance.hpp"
#include "Vanta/Scripts/CSharp/Interface.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/threads.h>

#ifdef VANTA_DEBUG
    #include <mono/metadata/mono-debug.h>
#endif

namespace Vanta {
    namespace Scripts {

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
        }

        void CSharpScriptEngine::Init() {
            VANTA_PROFILE_FUNCTION();

            InitMono();
            CreateAppDomain();

            CSharpInterface::RegisterFunctions();

            Path coreAssemblyPath = EngineScriptCorePath();
            if (!LoadCoreAssembly(coreAssemblyPath)) {
                VANTA_CORE_CRITICAL("Failed to load script core assembly!");
                return;
            }

            CSharpInterface::RegisterComponents();
        }

        void CSharpScriptEngine::Shutdown() {
            VANTA_PROFILE_FUNCTION();
            ShutdownMono();
        }

        void CSharpScriptEngine::InitMono() {
            VANTA_PROFILE_FUNCTION();

            mono_set_assemblies_path("mono/4.5");

            // TODO: Move to config variable
#ifdef VANTA_DEBUG
            const char* argv[] = {
                "--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebug.log",
                "--soft-breakpoints"
            };

            mono_jit_parse_options(2, (char**)argv);
            mono_debug_init(MONO_DEBUG_FORMAT_MONO);
#endif

            m_RootDomain = mono_jit_init("VantaJIT");
            VANTA_CORE_ASSERT(m_RootDomain, "Failed to initialize Mono JIT runtime!");

#ifdef VANTA_DEBUG
            mono_debug_domain_create(m_RootDomain);
#endif

            mono_thread_set_main(mono_thread_current());
        }

        void CSharpScriptEngine::ShutdownMono() {
            VANTA_PROFILE_FUNCTION();
            DestroyAppDomain();
            mono_jit_cleanup(m_RootDomain);
            m_RootDomain = nullptr;
        }

        void CSharpScriptEngine::CreateAppDomain() {
            VANTA_PROFILE_FUNCTION();
            std::string domainName = "VantaScripts";
            m_AppDomain = mono_domain_create_appdomain(domainName.data(), nullptr);
            mono_domain_set(m_AppDomain, true);
        }

        void CSharpScriptEngine::DestroyAppDomain() {
            VANTA_PROFILE_FUNCTION();
            mono_domain_set(mono_get_root_domain(), false);
            mono_domain_unload(m_AppDomain);
            m_AppDomain = nullptr;
        }

        bool CSharpScriptEngine::LoadCoreAssembly(const Path& filepath) {
            VANTA_PROFILE_FUNCTION();

            if (!std::filesystem::exists(filepath))
                return false;

            // Create copy, so original can be monitored for overwrite
            Path pdbFilepath = filepath;
            pdbFilepath.replace_extension(".pdb");

            Path activeDirectory = filepath.parent_path() / "Active";
            Path copyDllFilepath = activeDirectory / filepath.filename();
            Path copyPdbFilepath = activeDirectory / pdbFilepath.filename();

            std::filesystem::create_directories(activeDirectory);

            std::filesystem::remove_all(copyDllFilepath);
            std::filesystem::copy_file(filepath, copyDllFilepath);

            if (std::filesystem::exists(pdbFilepath)) {
                std::filesystem::remove_all(copyPdbFilepath);
                std::filesystem::copy_file(pdbFilepath, copyPdbFilepath);
            }

            // Load assembly
            m_CoreAssemblyFilepath = filepath;
            m_CoreAssembly = detail::LoadMonoAssembly(copyDllFilepath);
            if (!m_CoreAssembly)
                return false;

            m_CoreAssemblyImage = mono_assembly_get_image(m_CoreAssembly);

            // Save entity class
            m_EntityBaseClass = CSharpScriptClass(m_CoreAssemblyImage, "Vanta", "Entity", {});

            return true;
        }

        static std::unordered_map<std::string, bool> s_FileReloadPending = {};

        static void OnAppAssemblyFileChange(const std::string& filepath, const filewatch::Event type) {
            if (!s_FileReloadPending[filepath] && (type == filewatch::Event::added || type == filewatch::Event::modified)) {
                s_FileReloadPending[filepath] = true;

                // TODO: Figure out better way to avoid simultaneous read/write
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(250ms);

                // Queue app assembly reload with main thread
                Engine::Get().SubmitToMainThread([]() {
                    CSharpScriptEngine::Get().ReloadAssembly();
                });
            }
        }

        bool CSharpScriptEngine::LoadAppAssembly(const Path& filepath) {
            VANTA_PROFILE_FUNCTION();

            if (!std::filesystem::exists(filepath))
                return false;

            // Remove file watcher
            m_AppAssemblyFileWatcher.reset();

            // Create copy, so original can be monitored for overwrite
            Path pdbFilepath = filepath;
            pdbFilepath.replace_extension(".pdb");

            Path activeDirectory = filepath.parent_path() / "Active";
            Path copyDllFilepath = activeDirectory / filepath.filename();
            Path copyPdbFilepath = activeDirectory / pdbFilepath.filename();

            std::filesystem::create_directories(activeDirectory);

            std::filesystem::remove_all(copyDllFilepath);
            std::filesystem::copy_file(filepath, copyDllFilepath);

            if (std::filesystem::exists(pdbFilepath)) {
                std::filesystem::remove_all(copyPdbFilepath);
                std::filesystem::copy_file(pdbFilepath, copyPdbFilepath);
            }

            // Load assembly
            m_AppAssembly = detail::LoadMonoAssembly(copyDllFilepath);
            if (!m_AppAssembly)
                return false;

            m_AppAssemblyImage = mono_assembly_get_image(m_AppAssembly);

            // Get needed data from assembly
            InspectAssemblyImage(m_AppAssemblyImage);

            // Attach file watcher
            std::string filepathStr = filepath.string();
            m_AppAssemblyFileWatcher = NewBox<IO::FileWatcher>(filepathStr, OnAppAssemblyFileChange);
            s_FileReloadPending[filepathStr] = false;

            return true;
        }

        void CSharpScriptEngine::ReloadAssembly() {
            VANTA_PROFILE_FUNCTION();

            VANTA_CORE_INFO("Reloading C# script assembly!");

            DestroyAppDomain();
            CreateAppDomain();

            // Load assemblies
            if (!LoadCoreAssembly(m_CoreAssemblyFilepath)) {
                VANTA_CORE_CRITICAL("Failed to load script core assembly!");
                return;
            }
            Path appAssemblyPath = ProjectScriptLibraryPath();
            if (!LoadAppAssembly(appAssemblyPath)) {
                VANTA_CORE_CRITICAL("Failed to load app script assembly!");
                return;
            }

            // Re-register components in new domain
            CSharpInterface::RegisterComponents();
        }

        void CSharpScriptEngine::InspectAssemblyImage(MonoImage* image) {
            VANTA_PROFILE_FUNCTION();

            m_EntityClasses.clear();

            const MonoTableInfo* typeDefs = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
            int32 typeCount = mono_table_info_get_rows(typeDefs);

            for (int32 i = 0; i < typeCount; i++) {
                uint32 cols[MONO_TYPEDEF_SIZE];
                mono_metadata_decode_row(typeDefs, i, cols, MONO_TYPEDEF_SIZE);

                const char* namespaceName = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
                const char* className = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

                MonoClass* klass = mono_class_from_name(image, namespaceName, className);

                if (klass == m_EntityBaseClass)
                    continue;

                bool isEntity = mono_class_is_subclass_of(klass, (MonoClass*)m_EntityBaseClass, false);
                if (!isEntity)
                    continue;

                // Save classes that derive our `Entity` class
                std::string fullName = (strlen(namespaceName) != 0) ? FMT("{}.{}", namespaceName, className) : className;

                std::vector<Ref<ScriptField>> classFields;

                // Save public fields
                void* fieldIterator = nullptr;
                while (MonoClassField* field = mono_class_get_fields(klass, &fieldIterator)) {
                    auto flags = mono_field_get_flags(field);

                    bool isPublic = flags & FIELD_ATTRIBUTE_PUBLIC;
                    if (!isPublic)
                        continue;

                    const char* name = mono_field_get_name(field);
                    MonoType* type = mono_field_get_type(field);
                    Ref<ScriptField> classField = NewRef<CSharpScriptField>(name, type, field);
                    classFields.push_back(classField);
                }

                Ref<CSharpScriptClass> scriptClass = NewRef<CSharpScriptClass>(image, namespaceName, className, std::move(classFields));
                m_EntityClasses[fullName] = scriptClass;
            }
        }

        void CSharpScriptEngine::RuntimeBegin(Scene* context) {
            VANTA_PROFILE_FUNCTION();
            m_SceneContext = context;
        }

        void CSharpScriptEngine::RuntimeEnd() {
            VANTA_PROFILE_FUNCTION();
            m_SceneContext = nullptr;
        }

        Ref<ScriptInstance> CSharpScriptEngine::Instantiate(std::string fullName, Entity entity) {
            VANTA_PROFILE_FUNCTION();
            VANTA_CORE_ASSERT(EntityClassExists(fullName), "Invalid class!");
            VANTA_CORE_ASSERT(entity, "Invalid entity!");

            Ref<ScriptInstance> instance = NewRef<ScriptInstance>(GetEntityClass(fullName), entity);

            // Set variables modified in editor
            UUID entityId = entity.GetUUID();
            for (auto& [name, field] : m_EntityFieldInstances[entityId]) {
                instance->WriteFieldValue(name, field->GetFieldData());
            }

            return instance;
        }

        MonoObject* CSharpScriptEngine::CreateObject(MonoClass* klass) {
            VANTA_CORE_ASSERT(klass, "Invalid class!");
            return mono_object_new(m_AppDomain, klass);
        }

        bool CSharpScriptEngine::EntityClassExists(const std::string& fullName) const {
            return m_EntityClasses.contains(fullName);
        }

        Ref<ScriptClass> CSharpScriptEngine::GetEntityClass(const std::string& fullName) const {
            auto it = m_EntityClasses.find(fullName);
            if (it == m_EntityClasses.end()) {
                VANTA_CORE_WARN("Script class '{}' does not exist!", fullName);
                return nullptr;
            }
            return it->second;
        }

        const CSharpScriptClass& CSharpScriptEngine::GetEntityClass() const {
            return m_EntityBaseClass;
        }

        std::unordered_map<std::string, Box<ScriptFieldInstance>>& CSharpScriptEngine::GetFieldInstances(Entity entity) {
            VANTA_CORE_ASSERT(entity, "Invalid entity!");
            return m_EntityFieldInstances[entity.GetUUID()];
        }

        void CSharpScriptEngine::ClearFieldInstances() {
            m_EntityFieldInstances.clear();
        }
    
        Path CSharpScriptEngine::EngineScriptCorePath() {
            return Engine::RuntimeScriptDirectory(Scripts::ScriptType::CSharp) / "Vanta-ScriptCore-CSharp.dll";
        }

        Path CSharpScriptEngine::ProjectScriptLibraryPath() {
            return Project::GetScriptDirectory(Scripts::ScriptType::CSharp) / "Binaries" / "Scripts_CSharp.dll";
        }
    }
}
