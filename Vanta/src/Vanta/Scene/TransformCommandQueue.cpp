#include "vantapch.hpp"
#include "Vanta/Scene/TransformCommandQueue.hpp"
#include "Vanta/Scene/Components/TransformComponent.hpp"

namespace Vanta {

    // ---------------------------------------------------------------------------
    // Enqueue
    // ---------------------------------------------------------------------------

    void TransformCommandQueue::Enqueue(const SetPositionCommand& cmd) {
        m_SetPosition.push_back(cmd);
        ++m_Diagnostics.Enqueued;
    }

    void TransformCommandQueue::Enqueue(const SetRotationCommand& cmd) {
        m_SetRotation.push_back(cmd);
        ++m_Diagnostics.Enqueued;
    }

    void TransformCommandQueue::Enqueue(const SetScaleCommand& cmd) {
        m_SetScale.push_back(cmd);
        ++m_Diagnostics.Enqueued;
    }

    void TransformCommandQueue::Enqueue(const SetTransformCommand& cmd) {
        m_SetTransform.push_back(cmd);
        ++m_Diagnostics.Enqueued;
    }

    // ---------------------------------------------------------------------------
    // Apply
    // ---------------------------------------------------------------------------

    void TransformCommandQueue::Apply(entt::registry& registry, CommandPhase phase) {
        ApplyPositions (registry, phase);
        ApplyRotations (registry, phase);
        ApplyScales    (registry, phase);
        ApplyTransforms(registry, phase);
    }

    void TransformCommandQueue::ApplyPositions(entt::registry& registry, CommandPhase phase) {
        for (const auto& cmd : m_SetPosition) {
            if (cmd.Phase != phase)
                continue;

            auto* tc = registry.try_get<TransformComponent>(cmd.Entity);
            if (!tc) {
                ++m_Diagnostics.Dropped;
                VANTA_CORE_WARN("TransformCommandQueue: SetPosition dropped — entity {:x} has no TransformComponent",
                    (uint32_t)cmd.Entity);
                continue;
            }

            tc->SetPosition(cmd.Position);
            ++m_Diagnostics.Applied;
        }
        EraseCommandsForPhase(m_SetPosition, phase);
    }

    void TransformCommandQueue::ApplyRotations(entt::registry& registry, CommandPhase phase) {
        for (const auto& cmd : m_SetRotation) {
            if (cmd.Phase != phase)
                continue;

            auto* tc = registry.try_get<TransformComponent>(cmd.Entity);
            if (!tc) {
                ++m_Diagnostics.Dropped;
                VANTA_CORE_WARN("TransformCommandQueue: SetRotation dropped — entity {:x} has no TransformComponent",
                    (uint32_t)cmd.Entity);
                continue;
            }

            tc->SetRotationRad(cmd.RotationRad);
            ++m_Diagnostics.Applied;
        }
        EraseCommandsForPhase(m_SetRotation, phase);
    }

    void TransformCommandQueue::ApplyScales(entt::registry& registry, CommandPhase phase) {
        for (const auto& cmd : m_SetScale) {
            if (cmd.Phase != phase)
                continue;

            auto* tc = registry.try_get<TransformComponent>(cmd.Entity);
            if (!tc) {
                ++m_Diagnostics.Dropped;
                VANTA_CORE_WARN("TransformCommandQueue: SetScale dropped — entity {:x} has no TransformComponent",
                    (uint32_t)cmd.Entity);
                continue;
            }

            tc->SetScale(cmd.Scale);
            ++m_Diagnostics.Applied;
        }
        EraseCommandsForPhase(m_SetScale, phase);
    }

    void TransformCommandQueue::ApplyTransforms(entt::registry& registry, CommandPhase phase) {
        for (const auto& cmd : m_SetTransform) {
            if (cmd.Phase != phase)
                continue;

            auto* tc = registry.try_get<TransformComponent>(cmd.Entity);
            if (!tc) {
                ++m_Diagnostics.Dropped;
                VANTA_CORE_WARN("TransformCommandQueue: SetTransform dropped — entity {:x} has no TransformComponent",
                    (uint32_t)cmd.Entity);
                continue;
            }

            tc->SetTransformRad(cmd.Position, cmd.RotationRad, cmd.Scale);
            ++m_Diagnostics.Applied;
        }
        EraseCommandsForPhase(m_SetTransform, phase);
    }

    // ---------------------------------------------------------------------------
    // Flush
    // ---------------------------------------------------------------------------

    void TransformCommandQueue::Flush() {
        m_SetPosition.clear();
        m_SetRotation.clear();
        m_SetScale.clear();
        m_SetTransform.clear();
    }

    // ---------------------------------------------------------------------------
    // Inspection helpers
    // ---------------------------------------------------------------------------

    usize TransformCommandQueue::PendingCount() const {
        return m_SetPosition.size()
             + m_SetRotation.size()
             + m_SetScale.size()
             + m_SetTransform.size();
    }

    usize TransformCommandQueue::PendingCount(CommandPhase phase) const {
        return PendingCountForPhase(m_SetPosition, phase)
             + PendingCountForPhase(m_SetRotation, phase)
             + PendingCountForPhase(m_SetScale, phase)
             + PendingCountForPhase(m_SetTransform, phase);
    }

} // namespace Vanta
