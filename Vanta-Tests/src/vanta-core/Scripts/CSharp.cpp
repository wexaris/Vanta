#include <vanta-test-utils/CoreTestsCommon.hpp>

namespace Testing {

    static bool EnsureSandboxCSharpAssemblyLoaded() {
        static bool loaded = false;
        if (loaded)
            return true;

        Path repoRoot = std::filesystem::current_path();
        for (int i = 0; i < 4; ++i)
            repoRoot = repoRoot.parent_path();

        IO::File projectFile(repoRoot / "Sandbox" / "Sandbox.vproj");
        if (!projectFile.Exists())
            return false;

        auto project = Project::Load(projectFile);
        if (!project)
            return false;

        Path appAssemblyPath = Project::GetScriptDirectory(Scripts::ScriptType::CSharp) / "Binaries" / "Scripts_CSharp.dll";
        IO::File appAssemblyFile(appAssemblyPath);
        if (!appAssemblyFile.Exists())
            return false;

        CSharp::ScriptEngine::ReloadAssembly();

        loaded = true;
        return true;
    }

    bool TestCSharpPlayerInstantiatesWithoutUuidConstructor() {
        TRUE_OR_FAIL(EnsureSandboxCSharpAssemblyLoaded());
        TRUE_OR_FAIL(CSharp::ScriptEngine::EntityClassExists("Sandbox.Player"));

        Scene scene;
        Entity player = scene.CreateEntity("Player");

        auto& script = scene.AddComponent<ScriptComponent>(player.GetHandle());
        script.ClassName = "Sandbox.Player";

        CSharp::ScriptEngine::RuntimeBegin(&scene);
        script.Create(player.GetHandle(), &scene);

        TRUE_OR_FAIL(script.Instance != nullptr);
        const void* runtimeInstance = script.Instance->GetRuntimeInstance();
        TRUE_OR_FAIL(runtimeInstance != nullptr);

        script.Destroy();
        CSharp::ScriptEngine::RuntimeEnd();
        return true;
    }

    bool TestCSharpPlayerLifecycleWithoutOnDestroy() {
        TRUE_OR_FAIL(EnsureSandboxCSharpAssemblyLoaded());

        Scene scene;
        Entity player = scene.CreateEntity("Player");

        auto& script = scene.AddComponent<ScriptComponent>(player.GetHandle());
        script.ClassName = "Sandbox.Player";

        scene.OnRuntimeBegin();
        TRUE_OR_FAIL(script.Instance != nullptr);
        scene.OnRuntimeEnd();

        return true;
    }
}
