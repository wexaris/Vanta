#define VANTA_CUSTOM_LOG_SINKS
#include <vanta-test-utils/CoreTestsCommon.hpp>

#include "Math/Math.cpp"
#include "Fibers/Fibers.cpp"
#include "Events/Events.cpp"
#include "Scene/SceneRegistry.cpp"
#include "Project/Project.cpp"
#include "Scene/TransformCommandQueue.cpp"
#include "Scripts/CSharp.cpp"

using namespace Testing;

int main(int argc, char** argv) {
    std::filesystem::current_path(std::filesystem::absolute(argv[0]).parent_path());
    Log::Init("Vanta-Tests.log");

    EngineParams engineParams;
    engineParams.CommandLineArgs = CommandLineArguments(argc, argv);
    engineParams.Window.Title = "Vanta-Tests";
    engineParams.Window.Width = 1;
    engineParams.Window.Height = 1;
    Engine engine(engineParams);

    TestSet testMath("Math", { { "MathDecompose", TestMathDecompose } });
    TestSet testFibers("Fibers", { { "Fibers", TestFibers } });
    TestSet testEvents("Events", { { "Events", TestEvents } });
    TestSet testSceneRegistry("SceneRegistry", {
        { "BasicOperations", TestSceneRegistryBasics },
        { "ReplacementSemantics", TestSceneRegistryReplacementSemantics },
        { "TransformCommandsApplyByPhase", TestSceneTransformCommandsApplyByPhase },
        { "FlushClearsPendingCommands", TestSceneFlushClearsPendingCommands },
    });

    TestSet testProjectScaffolding("ProjectScaffolding", { { "ScriptProjectScaffolding", TestScriptProjectScaffolding } });

    TestSet testCommandQueue("TransformCommandQueue", {
        { "EnqueueSingleCommand",           EnqueueSingleCommand           },
        { "EnqueueMultipleCommands",         EnqueueMultipleCommands         },
        { "ApplyUpdatesRegistry",            ApplyUpdatesRegistry            },
        { "ApplyOnlyConsumesMatchingPhase",  ApplyOnlyConsumesMatchingPhase  },
        { "ScriptPhaseVisibleBeforePhysics", ScriptPhaseVisibleBeforePhysics },
        { "LastWriterWinsForSameEntityField", LastWriterWinsForSameEntityField },
        { "InvalidEntityIsDropped",          InvalidEntityIsDropped          },
        { "FlushClearsPendingCommands",      FlushClearsPendingCommands      },
        { "DeterministicRepeatedSequence",   DeterministicRepeatedSequence   },
        { "MultiEntityOrderingIsStable",     MultiEntityOrderingIsStable     },
        { "SetTransformAppliesAllFields",    SetTransformAppliesAllFields    },
        { "DiagnosticsAccumulateAndReset",   DiagnosticsAccumulateAndReset   },
    });

    TestSet testCSharpScripts("CSharpScripts", {
        { "PlayerInstantiatesWithoutUuidConstructor", TestCSharpPlayerInstantiatesWithoutUuidConstructor },
        { "PlayerLifecycleWithoutOnDestroy", TestCSharpPlayerLifecycleWithoutOnDestroy },
    });

    return (testMath.IsGood()
        && testFibers.IsGood()
        && testEvents.IsGood()
        && testSceneRegistry.IsGood()
        && testProjectScaffolding.IsGood()
        && testCommandQueue.IsGood()
        && testCSharpScripts.IsGood()) ? 0 : 1;
}
