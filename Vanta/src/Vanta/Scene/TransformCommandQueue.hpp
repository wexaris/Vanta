#pragma once
#include "Vanta/Scene/CommandQueueCommon.hpp"
#include "Vanta/Scene/Components/TransformComponent.hpp"

#include <entt/entt.hpp>
#include <vector>

namespace Vanta {

    // ---------------------------------------------------------------------------
    // Typed transform command payloads
    // ---------------------------------------------------------------------------

    /// Base data carried by every transform command.
    using TransformCommandBase = SceneCommandMeta;

    /// Overwrite position only.
    struct SetPositionCommand : TransformCommandBase {
        glm::vec3 Position = {};
    };

    /// Overwrite rotation only (radians).
    struct SetRotationCommand : TransformCommandBase {
        glm::vec3 RotationRad = {};
    };

    /// Overwrite scale only.
    struct SetScaleCommand : TransformCommandBase {
        glm::vec3 Scale = {};
    };

    /// Overwrite full transform (position, rotation in radians, scale).
    struct SetTransformCommand : TransformCommandBase {
        glm::vec3 Position    = {};
        glm::vec3 RotationRad = {};
        glm::vec3 Scale       = {};
    };

    // ---------------------------------------------------------------------------
    // TransformCommandQueue
    // ---------------------------------------------------------------------------

    /// <summary>
    /// Scene-owned command queue for deferred transform mutations.
    ///
    /// Producers (scripts, physics writeback, editor tools) enqueue typed
    /// commands during their respective update phases.  The owning Scene
    /// applies the queue at explicit synchronisation points — before physics
    /// reads (Script phase) and before render (Physics / Editor phase) — so
    /// that every system always reads committed, consistent component state.
    ///
    /// Thread-safety:
    ///   Enqueue is designed to be called from multiple producer threads via
    ///   thread-local staging buffers.  Apply must only be called on the
    ///   scene's owning thread.
    ///
    /// Conflict resolution:
    ///   Within a phase, commands targeting the same entity and field are
    ///   resolved last-writer-wins in stable insertion order.
    ///
    /// Extensibility:
    ///   To support a new transform field, add a typed command struct above
    ///   and a corresponding overload of Enqueue / Apply handler below.
    ///   Non-transform component domains follow the same pattern in their
    ///   own queue types and are composed into the Scene alongside this one.
    /// </summary>
    class TransformCommandQueue {
    public:
        TransformCommandQueue()  = default;
        ~TransformCommandQueue() = default;

        TransformCommandQueue(const TransformCommandQueue&)            = delete;
        TransformCommandQueue& operator=(const TransformCommandQueue&) = delete;

        // ------------------------------------------------------------------
        // Enqueue — called by producers during update phases
        // ------------------------------------------------------------------

        void Enqueue(const SetPositionCommand&  cmd);
        void Enqueue(const SetRotationCommand&  cmd);
        void Enqueue(const SetScaleCommand&     cmd);
        void Enqueue(const SetTransformCommand& cmd);

        // ------------------------------------------------------------------
        // Apply — called exclusively by Scene at synchronisation points
        // ------------------------------------------------------------------

        /// Apply all commands whose phase matches the given mask, then clear
        /// those commands.  registry is the scene's canonical component store.
        ///
        /// Commands for unrecognised or invalid entities increment the Dropped
        /// counter and are silently skipped in release builds.
        void Apply(entt::registry& registry, CommandPhase phase);

        /// Discard all pending commands without applying them.
        /// Used on teardown or when a frame must be abandoned.
        void Flush();

        // ------------------------------------------------------------------
        // Diagnostics
        // ------------------------------------------------------------------

        const CommandQueueDiagnostics& GetDiagnostics() const { return m_Diagnostics; }

        /// Reset diagnostic counters; typically called at the start of each frame.
        void ResetDiagnostics() { m_Diagnostics.Reset(); }

        // ------------------------------------------------------------------
        // Inspection helpers (tests + profiler)
        // ------------------------------------------------------------------

        /// Total number of commands currently pending (all phases combined).
        usize PendingCount() const;

        /// Number of commands pending for a specific phase.
        usize PendingCount(CommandPhase phase) const;

    private:
        // Each command variant gets its own vector so Apply can iterate a
        // single type without a polymorphic dispatch, and memory layout stays
        // contiguous and cache-friendly.
        std::vector<SetPositionCommand>  m_SetPosition;
        std::vector<SetRotationCommand>  m_SetRotation;
        std::vector<SetScaleCommand>     m_SetScale;
        std::vector<SetTransformCommand> m_SetTransform;

        CommandQueueDiagnostics m_Diagnostics;

        // Apply helpers — one per command type, templated on iteration.
        void ApplyPositions (entt::registry& registry, CommandPhase phase);
        void ApplyRotations (entt::registry& registry, CommandPhase phase);
        void ApplyScales    (entt::registry& registry, CommandPhase phase);
        void ApplyTransforms(entt::registry& registry, CommandPhase phase);
    };

} // namespace Vanta
