#pragma once

#include <entt/entt.hpp>

#include <algorithm>

namespace Vanta {

    /// Identifies which system produced a command.
    /// Used for diagnostics, tracing, and future priority/conflict resolution.
    enum class CommandSource : uint8_t {
        NativeScript,
        CSharpScript,
        Physics,
        Editor,
        Serialization,
    };

    /// Identifies which frame phase a command belongs to.
    /// Phase order governs which batch is applied first each frame:
    ///   Script -> Physics -> Editor (editor mode only)
    enum class CommandPhase : uint8_t {
        Script,
        Physics,
        Editor,
    };

    /// Base metadata carried by every scene command.
    struct SceneCommandMeta {
        entt::entity  Entity = entt::null;
        CommandSource Source = CommandSource::NativeScript;
        CommandPhase  Phase  = CommandPhase::Script;
    };

    struct CommandQueueDiagnostics {
        uint32_t Enqueued = 0;  ///< Commands submitted this frame
        uint32_t Applied  = 0;  ///< Commands successfully written to components
        uint32_t Dropped  = 0;  ///< Commands rejected (missing entity / component)

        void Reset() { Enqueued = Applied = Dropped = 0; }
    };

    template<typename CmdVector>
    usize PendingCountForPhase(const CmdVector& vec, CommandPhase phase) {
        usize n = 0;
        for (const auto& cmd : vec) {
            if (cmd.Phase == phase)
                ++n;
        }
        return n;
    }

    template<typename CmdVector>
    void EraseCommandsForPhase(CmdVector& vec, CommandPhase phase) {
        vec.erase(
            std::remove_if(vec.begin(), vec.end(),
                [phase](const auto& cmd) { return cmd.Phase == phase; }),
            vec.end());
    }

} // namespace Vanta
