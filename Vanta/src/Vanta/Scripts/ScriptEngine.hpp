#pragma once
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {

    struct ScriptEngine {
        static void Init() {
            CSharp::ScriptEngine::Init();
            Native::ScriptEngine::Init();
        }

        static void Shutdown() {
            CSharp::ScriptEngine::Shutdown();
            Native::ScriptEngine::Shutdown();
        }

        static void ReloadAssemblies() {
            CSharp::ScriptEngine::ReloadAssembly();
            Native::ScriptEngine::ReloadAssembly();
        }

        static void RuntimeBegin(Scene* context) {
            CSharp::ScriptEngine::RuntimeBegin(context);
            Native::ScriptEngine::RuntimeBegin(context);
        }

        static void RuntimeEnd() {
            CSharp::ScriptEngine::RuntimeEnd();
            Native::ScriptEngine::RuntimeEnd();
        }

        static void ClearFieldInstances() {
            CSharp::ScriptEngine::ClearFieldInstances();
            Native::ScriptEngine::ClearFieldInstances();
        }
    };
}
