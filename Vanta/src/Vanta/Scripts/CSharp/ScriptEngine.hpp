#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/CSharp/Class.hpp"
#include "Vanta/Scripts/ScriptEngine.hpp"
#include "Vanta/Util/Singleton.hpp"

extern "C" {
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoDomain MonoDomain;
    typedef struct _MonoImage MonoImage;
}

namespace Vanta {

    class Scene;

    namespace Scripts {

        class ScriptInstance;

        class CSharpScriptEngine : public ScriptEngine, public Singleton<CSharpScriptEngine> {
        public:
            void Init() override;
            void Shutdown() override;

            void ReloadAssembly() override;

            void RuntimeBegin(Scene* scene) override;
            void RuntimeEnd() override;

            Ref<ScriptInstance> Instantiate(std::string fullName, Entity entity);

            bool EntityClassExists(const std::string& fullName) const;
            Ref<ScriptClass> GetEntityClass(const std::string& fullName) const;
            const CSharpScriptClass& GetEntityClass() const;

            Scene* GetContext()               { return m_SceneContext; }
            MonoImage* GetCoreAssemblyImage() { return m_CoreAssemblyImage; }

            std::unordered_map<std::string, Box<ScriptFieldInstance>>& GetFieldInstances(Entity entity);
            void ClearFieldInstances() override;

        private:
            friend class CSharpScriptClass;
            friend struct Interface;

            // Mono
            MonoDomain* m_RootDomain = nullptr;
            MonoDomain* m_AppDomain = nullptr;

            MonoAssembly* m_CoreAssembly = nullptr;
            MonoImage* m_CoreAssemblyImage = nullptr;

            MonoAssembly* m_AppAssembly = nullptr;
            MonoImage* m_AppAssemblyImage = nullptr;

            // Runtime
            Scene* m_SceneContext = nullptr;
            CSharpScriptClass m_EntityBaseClass;
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

            MonoObject* CreateObject(MonoClass* klass);

            static Path EngineScriptCorePath();
            static Path ProjectScriptLibraryPath();
        };
    }
}
