#pragma once

namespace Vanta {

    class Scene;

    namespace Scripts {
        
        class ScriptEngine {
        public:
            virtual ~ScriptEngine() = default;

            virtual void Init() = 0;
            virtual void Shutdown() = 0;

            virtual void ReloadAssembly() = 0;

            virtual void RuntimeBegin(Scene* context) = 0;
            virtual void RuntimeEnd() = 0;

            virtual void ClearFieldInstances() = 0;
        };
    }
}
