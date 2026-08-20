#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Scripts/Instance.hpp"
#include "Vanta/Scripts/Native/Class.hpp"
#include "Vanta/Scripts/Native/Field.hpp"
#include "Vanta/Scripts/Native/Interface.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"
#include "Vanta/Util/DynamicLibrary.hpp"

namespace Vanta {
    namespace Scripts {

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

        NativeScriptEngine::NativeScriptEngine() = default;
        NativeScriptEngine::~NativeScriptEngine() = default;

        void NativeScriptEngine::Init() {
            VANTA_PROFILE_FUNCTION();
        }

        void NativeScriptEngine::Shutdown() {
            VANTA_PROFILE_FUNCTION();
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
                    Scripts::NativeScriptEngine::Get().ReloadAssembly();
                });
            }
        }

        bool NativeScriptEngine::LoadAppAssembly(const Path& filepath) {
            VANTA_PROFILE_FUNCTION();

            if (!std::filesystem::exists(filepath))
                return false;

            // Remove file watcher
            m_AppAssemblyFileWatcher.reset();

            m_AppAssembly.reset();

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
            m_AppAssembly = NewBox<ScriptAssembly>(copyDllFilepath);
            if (!m_AppAssembly->IsLoaded())
                return false;

            // Get needed data from assembly
            InspectAssembly(m_AppAssembly.get());

            // Attach file watcher to original
            std::string filepathStr = filepath.string();
            m_AppAssemblyFileWatcher = NewBox<IO::FileWatcher>(filepathStr, OnAppAssemblyFileChange);
            s_FileReloadPending[filepathStr] = false;

            return true;
        }

        void NativeScriptEngine::ReloadAssembly() {
            VANTA_PROFILE_FUNCTION();

            VANTA_CORE_INFO("Reloading native script assembly!");

            Path appAssemblyPath = ProjectScriptLibraryPath();
            if (!LoadAppAssembly(appAssemblyPath)) {
                VANTA_CORE_CRITICAL("Failed to load app script assembly!");
                return;
            }

            NativeInterface::RegisterFunctions();
            NativeInterface::RegisterComponents();
        }

        void NativeScriptEngine::InspectAssembly(ScriptAssembly* assembly) {
            VANTA_PROFILE_FUNCTION();

            auto [data, class_count] = assembly->GetClassList();
            for (; class_count > 0; class_count--, data++) {
                const char* className = *data;

                // Save public fields
                auto [field, field_count] = assembly->GetClassFieldList(className);

                std::vector<Ref<ScriptField>> classFields;
                classFields.reserve(field_count);

                for (; field_count > 0; field_count--, field++) {
                    ScriptFieldType type = detail::NativeTypeToFieldType(field->Type);
                    Ref<ScriptField> classField = NewRef<NativeScriptField>(field->Name, type, field->Getter, field->Setter);
                    classFields.push_back(classField);
                }

                Ref<NativeScriptClass> scriptClass = NewRef<NativeScriptClass>(assembly, className, std::move(classFields));
                m_EntityClasses[className] = scriptClass;
            }
        }

        void NativeScriptEngine::RuntimeBegin(Scene* scene) {
            VANTA_PROFILE_FUNCTION();
            m_SceneContext = scene;
        }

        void NativeScriptEngine::RuntimeEnd() {
            VANTA_PROFILE_FUNCTION();
            m_SceneContext = nullptr;
        }

        void NativeScriptEngine::ReleaseObject(void* obj) const {
            m_AppAssembly->Destroy(obj);
        }

        bool NativeScriptEngine::EntityClassExists(const std::string& className) const {
            return m_EntityClasses.contains(className);
        }

        Ref<ScriptClass> NativeScriptEngine::GetEntityClass(const std::string& className) const {
            auto it = m_EntityClasses.find(className);
            if (it == m_EntityClasses.end()) {
                VANTA_CORE_WARN("Script class '{}' does not exist!", className);
                return nullptr;
            }
            return it->second;
        }

        Opt<ValueRef<const std::unordered_map<std::string, Box<ScriptFieldInstance>>>> NativeScriptEngine::GetFieldInstances(Entity entity) const {
            VANTA_CORE_ASSERT(entity, "Invalid entity!");
            auto iter = m_EntityFieldInstances.find(entity.GetUUID());
            if (iter == m_EntityFieldInstances.end()) return None;
            return std::cref(iter->second);
        }

        void NativeScriptEngine::SetFieldInstance(Entity entity, Box<ScriptFieldInstance> instance) {
            VANTA_CORE_ASSERT(entity, "Invalid entity!");
            m_EntityFieldInstances[entity.GetUUID()][instance->Field->Name] = std::move(instance);
        }

        void NativeScriptEngine::ClearFieldInstances() {
            m_EntityFieldInstances.clear();
        }

        Path NativeScriptEngine::ProjectScriptLibraryPath() {
            return Project::GetScriptDirectory(Scripts::ScriptType::Native) / "Binaries" / "Scripts_Native.dll";
        }
    }
}
