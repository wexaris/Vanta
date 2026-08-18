#include "vantapch.hpp"
#include "Vanta/Scripts/ScriptManager.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {
    namespace Scripts {

        void ScriptManager::Init() {
            CSharpScriptEngine::Get().Init();
            NativeScriptEngine::Get().Init();
        }

        void ScriptManager::Shutdown() {
            CSharpScriptEngine::Get().Shutdown();
            NativeScriptEngine::Get().Shutdown();
        }

        void ScriptManager::ReloadAssemblies() {
            CSharpScriptEngine::Get().ReloadAssembly();
            NativeScriptEngine::Get().ReloadAssembly();
        }

        void ScriptManager::RuntimeBegin(Scene* context) {
            CSharpScriptEngine::Get().RuntimeBegin(context);
            NativeScriptEngine::Get().RuntimeBegin(context);
        }

        void ScriptManager::RuntimeEnd() {
            CSharpScriptEngine::Get().RuntimeEnd();
            NativeScriptEngine::Get().RuntimeEnd();
        }

        void ScriptManager::ClearFieldInstances() {
            CSharpScriptEngine::Get().ClearFieldInstances();
            NativeScriptEngine::Get().ClearFieldInstances();
        }
    }
}
