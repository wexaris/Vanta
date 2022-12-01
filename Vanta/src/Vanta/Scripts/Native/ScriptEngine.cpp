#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Project/Project.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"
#include "Vanta/Util/PlatformUtils.hpp"

namespace Vanta {
    namespace Native {

        struct ScriptData {
            Box<ScriptAssembly> AppAssembly;

            std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;

            // Runtime
            Scene* SceneContext;

            // Editor
            Box<IO::FileWatcher> AppAssemblyFileWatcher;
            bool AppAssemblyReloadPending = false;
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
        }

        void ScriptEngine::InspectAssembly(ScriptAssembly* assembly) {
            VANTA_PROFILE_FUNCTION();

            auto [data, count] = assembly->GetClassList();
            for (; count > 0; count--, data++) {
                const char* className = *data;

                Ref<ScriptClass> scriptClass = NewRef<ScriptClass>(assembly, className);
                s_Data.EntityClasses[className] = scriptClass;
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
    }
}
