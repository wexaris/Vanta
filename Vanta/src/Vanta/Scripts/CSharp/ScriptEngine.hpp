#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/ScriptEngine.hpp"
#include "Vanta/Util/Singleton.hpp"

extern "C" {
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoDomain MonoDomain;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
}

namespace Vanta {
    namespace Scripts {

        class CSharpScriptClass;

        class CSharpScriptEngine : public ScriptEngine, public Singleton<CSharpScriptEngine> {
        public:
            void Init() override;
            void Shutdown() override;

            void ReloadAssembly() override;

            void RuntimeBegin(Scene* scene) override;
            void RuntimeEnd() override;

            bool EntityClassExists(const std::string& fullName) const override;
            Ref<ScriptClass> GetEntityClass(const std::string& fullName) const override;
            const Ref<CSharpScriptClass>& GetEntityClass() const { return m_EntityBaseClass; }

            Scene* GetContext() const               { return m_SceneContext; }
            MonoImage* GetCoreAssemblyImage() const { return m_CoreAssemblyImage; }

            Opt<ValueRef<const std::unordered_map<std::string, Box<ScriptFieldInstance>>>> GetFieldInstances(Entity entity) const override;
            void SetFieldInstance(Entity entity, Box<ScriptFieldInstance> instance) override;
            void ClearFieldInstances() override;

        protected:
            friend class Singleton<CSharpScriptEngine>;

            CSharpScriptEngine();
            ~CSharpScriptEngine();

        private:
            friend struct Interface;
            friend class CSharpScriptClass;
            friend class CSharpScriptInstanceHandle;

            // Mono
            MonoDomain* m_RootDomain = nullptr;
            MonoDomain* m_AppDomain = nullptr;

            MonoAssembly* m_CoreAssembly = nullptr;
            MonoImage* m_CoreAssemblyImage = nullptr;

            MonoAssembly* m_AppAssembly = nullptr;
            MonoImage* m_AppAssemblyImage = nullptr;

            // Runtime
            Scene* m_SceneContext = nullptr;
            Ref<CSharpScriptClass> m_EntityBaseClass;
            std::unordered_map<std::string, Ref<CSharpScriptClass>> m_EntityClasses;

            // Editor - reload
            Path m_CoreAssemblyFilepath;
            Box<IO::FileWatcher> m_AppAssemblyFileWatcher = nullptr;

            // Editor - fields
            std::unordered_map<UUID, std::unordered_map<std::string, Box<ScriptFieldInstance>>> m_EntityFieldInstances;

            void InitMono();
            void ShutdownMono();

            void CreateAppDomain();
            void DestroyAppDomain();

            bool LoadCoreAssembly(const Path& filepath);
            bool LoadAppAssembly(const Path& filepath);
            void InspectAssemblyImage(MonoImage* image);

            uint32 CreateObject(MonoClass* klass) const;
            MonoObject* GetObjectByHandle(uint32 handle) const;
            void ReleaseObject(uint32 handle) const;

            static Path EngineScriptCorePath();
            static Path ProjectScriptLibraryPath();
        };
    }
}
