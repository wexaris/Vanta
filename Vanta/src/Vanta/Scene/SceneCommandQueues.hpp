#pragma once

#include "Vanta/Scene/TransformCommandQueue.hpp"

namespace Vanta {

    class SceneCommandQueues {
    public:
        void EnqueueTransformCommand(const SetPositionCommand& command) { m_TransformCommands.Enqueue(command); }
        void EnqueueTransformCommand(const SetRotationCommand& command) { m_TransformCommands.Enqueue(command); }
        void EnqueueTransformCommand(const SetScaleCommand& command) { m_TransformCommands.Enqueue(command); }
        void EnqueueTransformCommand(const SetTransformCommand& command) { m_TransformCommands.Enqueue(command); }

        template<typename Registry>
        void ApplyPhase(Registry& registry, CommandPhase phase) {
            if constexpr (requires { registry.Raw(); }) {
                m_TransformCommands.Apply(registry.Raw(), phase);
            }
            else {
                m_TransformCommands.Apply(registry, phase);
            }
        }

        void Flush() {
            m_TransformCommands.Flush();
        }

        void ResetDiagnostics() {
            m_TransformCommands.ResetDiagnostics();
        }

        template<typename Registry>
        void ApplyTransformCommands(Registry& registry, CommandPhase phase) {
            ApplyPhase(registry, phase);
        }

        void FlushTransformCommands() { Flush(); }
        void ResetTransformCommandDiagnostics() { ResetDiagnostics(); }

        const CommandQueueDiagnostics& GetTransformCommandDiagnostics() const {
            return m_TransformCommands.GetDiagnostics();
        }

    private:
        TransformCommandQueue m_TransformCommands;
    };

} // namespace Vanta
