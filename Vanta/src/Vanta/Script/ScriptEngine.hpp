#pragma once

namespace Vanta {

    class ScriptEngine {
    public:
        static void Init();
        static void Shutdown();

    private:
        ScriptEngine() = delete;

        static void InitMono();
        static void ShutdownMono();
    };
}
