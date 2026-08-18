#pragma once

namespace  Vanta {

    class Scene;

    namespace Scripts {

        class ScriptManager {
        public:
            static void Init();
            static void Shutdown();
            static void ReloadAssemblies();
            static void RuntimeBegin(Scene* context);
            static void RuntimeEnd();
            static void ClearFieldInstances();
        };
    }
}
