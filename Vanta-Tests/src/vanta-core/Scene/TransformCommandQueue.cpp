#include <vanta-test-utils/CoreTestsCommon.hpp>

namespace Testing {

    static entt::entity MakeEntityWithTransform(entt::registry& reg) {
        auto e = reg.create();
        reg.emplace<TransformComponent>(e);
        return e;
    }

    bool EnqueueSingleCommand() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);

        q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, {1.f, 2.f, 3.f} });

        TRUE_OR_FAIL(q.PendingCount() == 1);
        TRUE_OR_FAIL(q.PendingCount(CommandPhase::Script) == 1);
        TRUE_OR_FAIL(q.PendingCount(CommandPhase::Physics) == 0);
        TRUE_OR_FAIL(q.GetDiagnostics().Enqueued == 1);
        return true;
    }

    bool EnqueueMultipleCommands() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);

        q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, {1.f, 0.f, 0.f} });
        q.Enqueue(SetScaleCommand{ {e, CommandSource::Physics, CommandPhase::Physics}, {2.f, 2.f, 2.f} });
        q.Enqueue(SetRotationCommand{ {e, CommandSource::Editor, CommandPhase::Editor}, {0.f, 0.f, 1.f} });

        TRUE_OR_FAIL(q.PendingCount() == 3);
        TRUE_OR_FAIL(q.PendingCount(CommandPhase::Script) == 1);
        TRUE_OR_FAIL(q.PendingCount(CommandPhase::Physics) == 1);
        TRUE_OR_FAIL(q.PendingCount(CommandPhase::Editor) == 1);
        TRUE_OR_FAIL(q.GetDiagnostics().Enqueued == 3);
        return true;
    }

    bool ApplyUpdatesRegistry() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);

        const glm::vec3 newPos{ 5.f, 6.f, 7.f };
        q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, newPos });
        q.Apply(reg, CommandPhase::Script);

        const auto& tc = reg.get<TransformComponent>(e);
        TRUE_OR_FAIL(tc.GetPosition() == newPos);
        TRUE_OR_FAIL(q.PendingCount() == 0);
        TRUE_OR_FAIL(q.GetDiagnostics().Applied == 1);
        TRUE_OR_FAIL(q.GetDiagnostics().Dropped == 0);
        return true;
    }

    bool ApplyOnlyConsumesMatchingPhase() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);

        q.Enqueue(SetScaleCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, {3.f, 3.f, 3.f} });
        q.Enqueue(SetScaleCommand{ {e, CommandSource::Physics, CommandPhase::Physics}, {9.f, 9.f, 9.f} });

        q.Apply(reg, CommandPhase::Script);

        const auto& tc = reg.get<TransformComponent>(e);
        TRUE_OR_FAIL(tc.GetScale() == glm::vec3(3.f, 3.f, 3.f));
        TRUE_OR_FAIL(q.PendingCount() == 1);
        TRUE_OR_FAIL(q.PendingCount(CommandPhase::Physics) == 1);
        return true;
    }

    bool ScriptPhaseVisibleBeforePhysics() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);

        const glm::vec3 scriptPos{ 10.f, 0.f, 0.f };
        q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, scriptPos });

        q.Apply(reg, CommandPhase::Script);

        const glm::vec3 readByPhysics = reg.get<TransformComponent>(e).GetPosition();
        TRUE_OR_FAIL(readByPhysics == scriptPos);

        const glm::vec3 physicsPos{ 10.f, 5.f, 0.f };
        q.Enqueue(SetPositionCommand{ {e, CommandSource::Physics, CommandPhase::Physics}, physicsPos });

        q.Apply(reg, CommandPhase::Physics);

        const glm::vec3 readByRender = reg.get<TransformComponent>(e).GetPosition();
        TRUE_OR_FAIL(readByRender == physicsPos);
        TRUE_OR_FAIL(q.PendingCount() == 0);
        return true;
    }

    bool LastWriterWinsForSameEntityField() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);

        const glm::vec3 first{ 1.f, 1.f, 1.f };
        const glm::vec3 second{ 9.f, 9.f, 9.f };

        q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, first });
        q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, second });

        q.Apply(reg, CommandPhase::Script);

        const auto& tc = reg.get<TransformComponent>(e);
        TRUE_OR_FAIL(tc.GetPosition() == second);
        return true;
    }

    bool InvalidEntityIsDropped() {
        TransformCommandQueue q;
        entt::registry reg;

        auto bare = reg.create();

        q.Enqueue(SetPositionCommand{ {bare, CommandSource::NativeScript, CommandPhase::Script}, {1.f, 2.f, 3.f} });
        q.Apply(reg, CommandPhase::Script);

        TRUE_OR_FAIL(q.GetDiagnostics().Dropped == 1);
        TRUE_OR_FAIL(q.GetDiagnostics().Applied == 0);
        TRUE_OR_FAIL(q.PendingCount() == 0);
        return true;
    }

    bool FlushClearsPendingCommands() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);

        q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, {1.f, 0.f, 0.f} });
        q.Enqueue(SetScaleCommand{ {e, CommandSource::Physics, CommandPhase::Physics}, {2.f, 2.f, 2.f} });
        q.Flush();

        TRUE_OR_FAIL(q.PendingCount() == 0);

        const auto& tc = reg.get<TransformComponent>(e);
        TRUE_OR_FAIL(tc.GetPosition() == glm::vec3(0.f));
        TRUE_OR_FAIL(tc.GetScale() == glm::vec3(1.f));
        return true;
    }

    bool DeterministicRepeatedSequence() {
        auto runFrame = [](const glm::vec3& pos) -> glm::vec3 {
            TransformCommandQueue q;
            entt::registry reg;
            auto e = reg.create();
            reg.emplace<TransformComponent>(e);

            q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, pos });
            q.Apply(reg, CommandPhase::Script);
            return reg.get<TransformComponent>(e).GetPosition();
        };

        const glm::vec3 input{ 3.f, 4.f, 5.f };
        TRUE_OR_FAIL(runFrame(input) == runFrame(input));
        TRUE_OR_FAIL(runFrame(input) == input);
        return true;
    }

    bool MultiEntityOrderingIsStable() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e1 = MakeEntityWithTransform(reg);
        auto e2 = MakeEntityWithTransform(reg);

        const glm::vec3 pos1{ 1.f, 0.f, 0.f };
        const glm::vec3 pos2{ 2.f, 0.f, 0.f };

        q.Enqueue(SetPositionCommand{ {e1, CommandSource::NativeScript, CommandPhase::Script}, pos1 });
        q.Enqueue(SetPositionCommand{ {e2, CommandSource::NativeScript, CommandPhase::Script}, pos2 });

        q.Apply(reg, CommandPhase::Script);

        TRUE_OR_FAIL(reg.get<TransformComponent>(e1).GetPosition() == pos1);
        TRUE_OR_FAIL(reg.get<TransformComponent>(e2).GetPosition() == pos2);
        return true;
    }

    bool SetTransformAppliesAllFields() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);

        const glm::vec3 pos{ 1.f, 2.f, 3.f };
        const glm::vec3 rot{ 0.f, 0.f, 1.5708f };
        const glm::vec3 scl{ 2.f, 2.f, 2.f };

        q.Enqueue(SetTransformCommand{ {e, CommandSource::Physics, CommandPhase::Physics}, pos, rot, scl });
        q.Apply(reg, CommandPhase::Physics);

        const auto& tc = reg.get<TransformComponent>(e);
        TRUE_OR_FAIL(tc.GetPosition() == pos);
        TRUE_OR_FAIL(tc.GetRotationRadians() == rot);
        TRUE_OR_FAIL(tc.GetScale() == scl);
        return true;
    }

    bool DiagnosticsAccumulateAndReset() {
        TransformCommandQueue q;
        entt::registry reg;
        auto e = MakeEntityWithTransform(reg);
        auto bare = reg.create();

        q.Enqueue(SetPositionCommand{ {e, CommandSource::NativeScript, CommandPhase::Script}, {1.f, 0.f, 0.f} });
        q.Enqueue(SetPositionCommand{ {bare, CommandSource::NativeScript, CommandPhase::Script}, {9.f, 0.f, 0.f} });

        q.Apply(reg, CommandPhase::Script);

        TRUE_OR_FAIL(q.GetDiagnostics().Enqueued == 2);
        TRUE_OR_FAIL(q.GetDiagnostics().Applied == 1);
        TRUE_OR_FAIL(q.GetDiagnostics().Dropped == 1);

        q.ResetDiagnostics();

        TRUE_OR_FAIL(q.GetDiagnostics().Enqueued == 0);
        TRUE_OR_FAIL(q.GetDiagnostics().Applied == 0);
        TRUE_OR_FAIL(q.GetDiagnostics().Dropped == 0);
        return true;
    }
}
