#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Native/Class.hpp"
#include "Vanta/Scripts/Native/Assembly.hpp"

namespace Vanta {
    namespace Native {

        class ScriptEngine {
        public:
            static void Init();
            static void Shutdown();

            static void ReloadAssembly();

            static void RuntimeBegin(Scene* scene);
            static void RuntimeEnd();

            static Ref<ScriptInstance> Instantiate(std::string fullName, Entity entity);

            static bool ClassExists(const std::string& className);
            static Ref<ScriptClass> GetClass(const std::string& className);

            static Scene* GetContext();
            static ScriptAssembly* GetAppAssembly();

        private:
            friend struct Interface;

            ScriptEngine() = delete;

            static bool LoadAppAssembly(const Path& filepath);

            static void InspectAssembly(ScriptAssembly* assembly);
        };
    }
}
