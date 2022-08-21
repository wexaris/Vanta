#include "Editor/EditorLayer.hpp"

namespace Vanta {
    namespace Editor {

        EditorLayer::EditorLayer()
            : Layer("Editor Layer") {}

        void EditorLayer::OnAttach() {

        }

        void EditorLayer::OnDetach() {

        }

        void EditorLayer::OnUpdate(double delta) {
            switch (m_State)
            {
            case State::Editor:
                m_ActiveScene.OnUpdateEditor(delta);
                break;
            case State::Simulation:
                m_ActiveScene.OnUpdateSimulation(delta);
                break;
            case State::Runtime:
                m_ActiveScene.OnUpdateRuntime(delta);
                break;
            default:
                VANTA_UNREACHABLE("Invalid application state!");
                break;
            }
        }

        void EditorLayer::OnGUIRender() {
            // Render editor menus
        }

        void EditorLayer::OnEvent(Event& e) {
            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<WindowResizeEvent>(EVENT_METHOD(EditorLayer::OnWindowResize));
        }

        bool EditorLayer::OnWindowResize(WindowResizeEvent& e) {
            m_ActiveScene.OnWindowResize(e);
            return true;
        }
    }
}
