#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"

namespace Vanta {
    extern Engine* CreateEngine(CommandLineArguments);

    static int main(int argc, char** argv) {
        Log::Init("Vanta.log");

        VANTA_PROFILE_BEGIN("Startup", "VantaProfile-Startup.json");
        auto args = CommandLineArguments(argc, argv);
        auto engine = CreateEngine(args);
        VANTA_PROFILE_END();

        VANTA_PROFILE_BEGIN("Runtime", "VantaProfile-Runtime.json");
        engine->Run();
        VANTA_PROFILE_END();

        VANTA_PROFILE_BEGIN("Shutdown", "VantaProfile-Shutdown.json");
        delete engine;
        VANTA_PROFILE_END();

        return 0;
    }
}

#ifdef VANTA_PLATFORM_WINDOWS
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, INT /*nCmdShow*/) {
    return Vanta::main(__argc, __argv);
}
int main(int argc, char* argv[]) {
    return Vanta::main(argc, argv);
}
#else
int main(int argc, char* argv[]) {
    return Vanta::main(argc, argv);
}
#endif
