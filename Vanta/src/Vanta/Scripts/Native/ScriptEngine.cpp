#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Scripts/Native/Field.hpp"
#include "Vanta/Scripts/Native/Interface.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"
#include "Vanta/Util/PlatformUtils.hpp"

namespace Vanta {
    namespace Native {

        namespace detail {
            static std::unordered_map<std::string, ScriptFieldType> s_NativeFieldTypeMap = {
                { "bool", ScriptFieldType::Bool },
                { "char", ScriptFieldType::Char },

                { "byte", ScriptFieldType::Int8 },
                { "int8", ScriptFieldType::Int8 },
                { "int16", ScriptFieldType::Int16 },
                { "int32", ScriptFieldType::Int32 },
                { "int64", ScriptFieldType::Int64 },
                { "int", ScriptFieldType::Int32 },

                { "uint8", ScriptFieldType::UInt8 },
                { "uint16", ScriptFieldType::UInt16 },
                { "uint32", ScriptFieldType::UInt32 },
                { "uint32", ScriptFieldType::UInt32 },
                { "uint64", ScriptFieldType::UInt64 },
                { "uint", ScriptFieldType::UInt32 },

                { "float", ScriptFieldType::Float },
                { "double", ScriptFieldType::Double },

                { "Vector2", ScriptFieldType::Vector2 },
                { "Vector3", ScriptFieldType::Vector3 },
                { "Vector4", ScriptFieldType::Vector4 },

                { "Entity", ScriptFieldType::Entity },
            };

            static ScriptFieldType NativeTypeToFieldType(const char* name) {
                auto it = s_NativeFieldTypeMap.find(name);
                if (it == s_NativeFieldTypeMap.end()) {
                    VANTA_CORE_ERROR("Invalid native field type: {}", name);
                    return ScriptFieldType::None;
                }
                return it->second;
            }
        }

        struct ScriptData {
            Box<ScriptAssembly> AppAssembly;

            std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;

            // Runtime
            Scene* SceneContext;

            // Editor
            Box<IO::FileWatcher> AppAssemblyFileWatcher;
            bool AppAssemblyReloadPending = false;

            std::unordered_map<UUID, std::unordered_map<std::string, Box<ScriptFieldInstance>>> EntityFieldInstances;
        };

        static ScriptData s_Data;

        void ScriptEngine::Init() {
            VANTA_PROFILE_FUNCTION();
        }

        void ScriptEngine::Shutdown() {
            VANTA_PROFILE_FUNCTION();
        }

        void OnAppAssemblyFileChange(const std::string&, const filewatch::Event type) {
            if (!s_Data.AppAssemblyReloadPending && type == filewatch::Event::modified) {
                s_Data.AppAssemblyReloadPending = true;

                // TODO: Figure out better way to avoid simultaneous read/write
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(250ms);

                // Queue app assembly reload with main thread
                Engine::Get().SubmitToMainThread([]() {
                    ScriptEngine::ReloadAssembly();
                });
            }
        }

        bool ScriptEngine::LoadAppAssembly(const Path& filepath) {
            VANTA_PROFILE_FUNCTION();

            if (!std::filesystem::exists(filepath))
                return false;

            // Remove file watcher
            s_Data.AppAssemblyFileWatcher.reset();

            s_Data.AppAssembly.reset();

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
            s_Data.AppAssembly = NewBox<ScriptAssembly>(copyDllFilepath);
            if (!s_Data.AppAssembly->IsLoaded())
                return false;

            // Get needed data from assembly
            InspectAssembly(s_Data.AppAssembly.get());

            // Attach file watcher to original
            s_Data.AppAssemblyFileWatcher = NewBox<IO::FileWatcher>(filepath.string(), OnAppAssemblyFileChange);
            s_Data.AppAssemblyReloadPending = false;

            return true;
        }

        void ScriptEngine::ReloadAssembly() {
            VANTA_PROFILE_FUNCTION();

            VANTA_CORE_INFO("Reloading native script assembly!");

            Path appAssemblyPath = Project::GetRootDirectory() / Project::GetActive()->GetConfig().NativeScriptAssemblyPath;
            if (!LoadAppAssembly(appAssemblyPath)) {
                VANTA_CORE_CRITICAL("Failed to load app script assembly!");
                return;
            }

            Interface::RegisterFunctions();
            Interface::RegisterComponents();
        }

        void ScriptEngine::InspectAssembly(ScriptAssembly* assembly) {
            VANTA_PROFILE_FUNCTION();

            auto [data, class_count] = assembly->GetClassList();
            for (; class_count > 0; class_count--, data++) {
                const char* className = *data;

                Ref<ScriptClass> scriptClass = NewRef<ScriptClass>(assembly, className);
                s_Data.EntityClasses[className] = scriptClass;

                // Save public fields
                auto [fields, field_count] = assembly->GetClassFieldList(className);
                for (; field_count > 0; field_count--, fields++) {
                    const ClassField& field = *fields;

                    ScriptFieldType type = detail::NativeTypeToFieldType(field.Type);
                    scriptClass->m_Fields[field.Name] = { field.Name, type, field.Getter, field.Setter };
                }
            }
        }

        void ScriptEngine::RuntimeBegin(Scene* scene) {
            VANTA_PROFILE_FUNCTION();
            s_Data.SceneContext = scene;
        }

        void ScriptEngine::RuntimeEnd() {
            VANTA_PROFILE_FUNCTION();
            s_Data.SceneContext = nullptr;
        }

        Ref<ScriptInstance> ScriptEngine::Instantiate(std::string className, Entity entity) {
            VANTA_PROFILE_FUNCTION();
            VANTA_CORE_ASSERT(ClassExists(className), "Invalid class!");
            VANTA_CORE_ASSERT(entity, "Invalid entity!");

            Ref<ScriptInstance> instance = NewRef<ScriptInstance>(GetClass(className), entity);

            // Set variables modified in editor
            UUID entityId = entity.GetUUID();
            for (auto& [name, field] : s_Data.EntityFieldInstances[entityId]) {
                instance->SetFieldValue_Impl(name, field->m_Buffer);
            }

            return instance;
        }

        bool ScriptEngine::ClassExists(const std::string& className) {
            return s_Data.EntityClasses.contains(className);
        }

        Ref<ScriptClass> ScriptEngine::GetClass(const std::string& className) {
            VANTA_CORE_ASSERT(ClassExists(className), "Invalid class!");
            return s_Data.EntityClasses.at(className);
        }

        Scene* ScriptEngine::GetContext() {
            return s_Data.SceneContext;
        }

        ScriptAssembly* ScriptEngine::GetAppAssembly() {
            return s_Data.AppAssembly.get();
        }

        std::unordered_map<std::string, Box<ScriptFieldInstance>>& ScriptEngine::GetFieldInstances(Entity entity) {
            VANTA_CORE_ASSERT(entity, "Invalid entity!");
            return s_Data.EntityFieldInstances[entity.GetUUID()];
        }

        void ScriptEngine::ClearFieldInstances() {
            s_Data.EntityFieldInstances.clear();
        }
    }
}
