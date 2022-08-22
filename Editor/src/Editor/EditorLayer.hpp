#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta {
    namespace Editor {

        class EditorLayer : public Layer {
        public:
            EditorLayer();
            virtual ~EditorLayer() = default;

            void OnAttach() override;
            void OnDetach() override;

            void OnUpdate(double delta) override;
            void OnGUIRender() override;

            void OnEvent(Event& e) override;
            
        private:
            enum class State {
                Editor,
                Simulation,
                Runtime,
            };

            State m_State = State::Editor;

            Vanta::Scene m_ActiveScene;

            bool OnWindowResize(WindowResizeEvent& e);
        };
    }
}
