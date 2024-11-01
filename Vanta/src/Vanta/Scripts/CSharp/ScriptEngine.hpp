#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/CSharp/Class.hpp"
#include "Vanta/Scripts/CSharp/Field.hpp"

extern "C" {
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
}

namespace Vanta {

    class Scene;

    namespace CSharp {

        class ScriptEngine {
        public:
            static void Init();
            static void Shutdown();

            static void ReloadAssembly();

            static void RuntimeBegin(Scene* scene);
            static void RuntimeEnd();

            static Ref<ScriptInstance> Instantiate(std::string fullName, Entity entity);

            static bool ClassExists(const std::string& fullName);
            static Ref<ScriptClass> GetClass(const std::string& fullName);
            static const ScriptClass& GetEntityClass();

            static Scene* GetContext();
            static MonoImage* GetCoreAssemblyImage();

            static std::unordered_map<std::string, Box<ScriptFieldInstance>>& GetFieldInstances(Entity entity);
            static void ClearFieldInstances();

        private:
            friend class ScriptClass;
            friend class ScriptInstance;
            friend struct Interface;

            ScriptEngine() = delete;

            static void InitMono();
            static void ShutdownMono();

            static void CreateAppDomain();
            static void DestroyAppDomain();

            static bool LoadCoreAssembly(const Path& filepath);
            static bool LoadAppAssembly(const Path& filepath);

            static void InspectAssemblyImage(MonoImage* image);

            static MonoObject* CreateObject(MonoClass* klass);

            static Path DefaultScriptCorePath();
        };
    }
}
