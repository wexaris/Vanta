#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/ScriptEngine.hpp"
#include "Vanta/Scripts/Native/Module/Assembly.hpp"
#include "Vanta/Util/Singleton.hpp"

namespace Vanta {
    namespace Scripts {

        class NativeScriptClass;

        class NativeScriptEngine : public ScriptEngine, public Singleton<NativeScriptEngine> {
        public:
            void Init() override;
            void Shutdown() override;

            void ReloadAssembly() override;

            void RuntimeBegin(Scene* scene) override;
            void RuntimeEnd() override;

            bool EntityClassExists(const std::string& className) const override;
            Ref<ScriptClass> GetEntityClass(const std::string& className) const override;

            Scene* GetContext() const              { return m_SceneContext; }
            ScriptAssembly* GetAppAssembly() const { return m_AppAssembly.get(); }

            Opt<ValueRef<const std::unordered_map<std::string, Box<ScriptFieldInstance>>>> GetFieldInstances(Entity entity) const override;
            void SetFieldInstance(Entity entity, Box<ScriptFieldInstance> instance) override;
            void ClearFieldInstances() override;

        protected:
            friend class Singleton<NativeScriptEngine>;

            NativeScriptEngine();
            ~NativeScriptEngine();

        private:
            friend struct Interface;
            friend class NativeScriptInstanceHandle;

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

            void ReleaseObject(void* obj) const;

            static Path ProjectScriptLibraryPath();
        };
    }
}
