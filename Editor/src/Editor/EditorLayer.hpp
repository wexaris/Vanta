#pragma once
#include <Vanta/Vanta.hpp>
#include <Vanta/Render/Framebuffer.hpp>

#include "Editor/CameraController.hpp"
#include "Editor/Window/Console.hpp"
#include "Editor/Window/ContentBrowser.hpp"
#include "Editor/Window/SceneHierarchy.hpp"

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
                Edit,
                Simulate,
                Play,
            };

            State m_State = State::Edit;
            bool m_IsPaused = false;

            Ref<Scene> m_ActiveScene;
            Ref<Scene> m_EditorScene;
            Path m_SceneFilepath;

            ViewportCameraController m_EditorCamera;

            SceneHierarchy m_ScenePanel;
            Box<ContentBrowser> m_ContentPanel;

            Ref<Framebuffer> m_Framebuffer;
            glm::vec2 m_ViewportSize = { 0.f, 0.f };
            glm::vec2 m_ViewportBounds[2];

            bool m_ViewportActive = false;
            bool m_ViewportFocused = false;
            bool m_ViewportHovered = false;

            bool m_ShowPhysicsColliders = false;

            Entity m_HoveredEntity;
            int m_GizmoType = -1;

            Ref<Texture2D> m_IconPlay;
            Ref<Texture2D> m_IconSimulate;
            Ref<Texture2D> m_IconPause;
            Ref<Texture2D> m_IconStep;
            Ref<Texture2D> m_IconStop;

            void RenderOverlay();

            void RenderToolbar();

            bool OnMouseButtonPress(MouseButtonPressEvent& e);
            bool OnKeyPress(KeyPressEvent&e);

            void OnPlay();
            void OnSimulate();
            void OnPause();
            void OnStop();

            void OnDuplicateEntity();

            bool NewProject();
            bool OpenProject();
            bool OpenProject(const Path& path);
            void SaveProject();

            void NewScene();
            void OpenScene();
            void OpenScene(const Path& path);
            void SaveScene();
            void SaveSceneAs();
        };
    }
}
