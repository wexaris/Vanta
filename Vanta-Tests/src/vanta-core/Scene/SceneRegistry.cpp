#include <vanta-test-utils/CoreTestsCommon.hpp>

namespace Testing {

    bool TestSceneRegistryBasics() {
        SceneRegistry reg;

        auto entity = reg.Create();
        TRUE_OR_FAIL(reg.IsValid(entity));
        TRUE_OR_FAIL(!reg.HasComponent<TransformComponent>(entity));

        auto& tr = reg.AddComponent<TransformComponent>(entity);
        TRUE_OR_FAIL(reg.HasComponent<TransformComponent>(entity));

        auto scale333 = glm::vec3{ 3, 3, 3 };
        tr.SetScale(scale333);

        auto* tc = reg.TryGetComponent<TransformComponent>(entity);
        TRUE_OR_FAIL(tc != nullptr);
        TRUE_OR_FAIL(tc->GetScale() == scale333);

        auto& trGet = reg.GetComponent<TransformComponent>(entity);
        TRUE_OR_FAIL(trGet.GetScale() == scale333);

        reg.RemoveComponent<TransformComponent>(entity);
        TRUE_OR_FAIL(!reg.HasComponent<TransformComponent>(entity));

        reg.Destroy(entity);
        TRUE_OR_FAIL(!reg.IsValid(entity));

        return true;
    }

    bool TestSceneRegistryReplacementSemantics() {
        SceneRegistry reg;
        auto entity = reg.Create();

        auto& original = reg.AddComponent<TransformComponent>(entity);
        original.SetPosition({ 1.f, 2.f, 3.f });
        original.SetScale({ 1.f, 1.f, 1.f });

        auto& replaced = reg.AddOrReplaceComponent<TransformComponent>(entity, TransformComponent{});
        replaced.SetPosition({ 4.f, 5.f, 6.f });
        replaced.SetScale({ 2.f, 2.f, 2.f });

        const auto* tc = reg.TryGetComponent<TransformComponent>(entity);
        TRUE_OR_FAIL(tc != nullptr);
        TRUE_OR_FAIL((tc->GetPosition() == glm::vec3{ 4.f, 5.f, 6.f }));
        TRUE_OR_FAIL((tc->GetScale() == glm::vec3{ 2.f, 2.f, 2.f }));

        return true;
    }

    bool TestSceneTransformCommandsApplyByPhase() {
        Scene scene;
        auto entity = scene.CreateEntity("TransformEntity");
        auto& transform = scene.GetComponent<TransformComponent>(entity.GetHandle());

        const glm::vec3 scriptPos{ 1.f, 2.f, 3.f };
        const glm::vec3 physicsPos{ 4.f, 5.f, 6.f };

        scene.EnqueueTransformCommand(SetPositionCommand{
            { entity.GetHandle(), CommandSource::NativeScript, CommandPhase::Script },
            scriptPos
        });
        scene.EnqueueTransformCommand(SetPositionCommand{
            { entity.GetHandle(), CommandSource::Physics, CommandPhase::Physics },
            physicsPos
        });

        scene.ResetTransformCommandDiagnostics();
        scene.ApplyTransformCommands(CommandPhase::Script);

        TRUE_OR_FAIL((transform.GetPosition() == scriptPos));
        TRUE_OR_FAIL((scene.GetComponent<TransformComponent>(entity.GetHandle()).GetPosition() == scriptPos));

        scene.ApplyTransformCommands(CommandPhase::Physics);

        TRUE_OR_FAIL((scene.GetComponent<TransformComponent>(entity.GetHandle()).GetPosition() == physicsPos));
        TRUE_OR_FAIL((scene.GetTransformCommandDiagnostics().Applied == 2));

        return true;
    }

    bool TestSceneFlushClearsPendingCommands() {
        Scene scene;
        auto entity = scene.CreateEntity("FlushEntity");
        auto& transform = scene.GetComponent<TransformComponent>(entity.GetHandle());

        const glm::vec3 queuedPosition{ 9.f, 8.f, 7.f };
        scene.EnqueueTransformCommand(SetPositionCommand{
            { entity.GetHandle(), CommandSource::NativeScript, CommandPhase::Script },
            queuedPosition
        });

        scene.FlushCommands();
        scene.ApplyTransformCommands(CommandPhase::Script);

        TRUE_OR_FAIL((transform.GetPosition() == glm::vec3(0.f)));
        TRUE_OR_FAIL((scene.GetTransformCommandDiagnostics().Applied == 0));
        TRUE_OR_FAIL((scene.GetTransformCommandDiagnostics().Dropped == 0));

        return true;
    }
}
