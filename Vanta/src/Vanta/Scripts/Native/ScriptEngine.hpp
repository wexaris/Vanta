#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/ScriptEngine.hpp"
#include "Vanta/Scripts/Native/Class.hpp"
#include "Vanta/Scripts/Native/Module/Assembly.hpp"
#include "Vanta/Util/Singleton.hpp"

namespace Vanta {

    class Scene;

    namespace Scripts {

        class ScriptInstance;

        class NativeScriptEngine : public ScriptEngine, public Singleton<NativeScriptEngine> {
        public:
            void Init() override;
            void Shutdown() override;

            void ReloadAssembly() override;

            void RuntimeBegin(Scene* scene) override;
            void RuntimeEnd() override;

            Ref<ScriptInstance> Instantiate(std::string fullName, Entity entity);

            bool EntityClassExists(const std::string& className);
            Ref<ScriptClass> GetEntityClass(const std::string& className);

            Scene* GetContext()              { return m_SceneContext; }
            ScriptAssembly* GetAppAssembly() { return m_AppAssembly.get(); }

            std::unordered_map<std::string, Box<ScriptFieldInstance>>& GetFieldInstances(Entity entity);
            void ClearFieldInstances() override;

        private:
            friend struct Interface;

            // Assembly
            Box<ScriptAssembly> m_AppAssembly;

            // Runtime
            Scene* m_SceneContext = nullptr;
            std::unordered_map<std::string, Ref<NativeScriptClass>> m_EntityClasses;

            // Editor - reload
            Box<IO::FileWatcher> m_AppAssemblyFileWatcher;

            // Editor - fields
            std::unordered_map<UUID, std::unordered_map<std::string, Box<ScriptFieldInstance>>> m_EntityFieldInstances;

            bool LoadAppAssembly(const Path& filepath);
            void InspectAssembly(ScriptAssembly* assembly);

            static Path ProjectScriptLibraryPath();
        };
    }
}
