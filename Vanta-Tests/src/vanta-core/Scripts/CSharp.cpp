#include <vanta-test-utils/CoreTestsCommon.hpp>
#include <Vanta/Scripts/Instance.hpp>

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

        Scripts::CSharpScriptEngine::Get().ReloadAssembly();

        loaded = true;
        return true;
    }

    bool TestCSharpPlayerInstantiatesWithoutUuidConstructor() {
        TRUE_OR_FAIL(EnsureSandboxCSharpAssemblyLoaded());

        Scripts::CSharpScriptEngine& engine = Scripts::CSharpScriptEngine::Get();
        TRUE_OR_FAIL(engine.EntityClassExists("Sandbox.Player"));

        Scene scene;
        Entity player = scene.CreateEntity("Player");

        auto& script = scene.AddComponent<CSharpScriptComponent>(player.GetHandle());
        script.ClassName = "Sandbox.Player";

        engine.RuntimeBegin(&scene);
        script.Create(player.GetHandle(), &scene);

        TRUE_OR_FAIL(script.Instance != nullptr);
        const void* runtimeObject = script.Instance->GetRuntimeObject();
        TRUE_OR_FAIL(runtimeObject != nullptr);

        script.Destroy();
        engine.RuntimeEnd();
        return true;
    }

    bool TestCSharpPlayerLifecycleWithoutOnDestroy() {
        TRUE_OR_FAIL(EnsureSandboxCSharpAssemblyLoaded());

        Scene scene;
        Entity player = scene.CreateEntity("Player");

        auto& script = scene.AddComponent<CSharpScriptComponent>(player.GetHandle());
        script.ClassName = "Sandbox.Player";

        scene.OnRuntimeBegin();
        TRUE_OR_FAIL(script.Instance != nullptr);
        scene.OnRuntimeEnd();

        return true;
    }
}
