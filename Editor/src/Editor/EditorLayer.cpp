#include "Editor/EditorLayer.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

namespace Vanta {
    namespace Editor {

        EditorLayer::EditorLayer() :
            Layer("Editor Layer") {}

        void EditorLayer::OnAttach() {
            VANTA_PROFILE_FUNCTION();

            m_IconPlay = Texture2D::Create("Icons/PlayButton.png");
            m_IconSimulate = Texture2D::Create("Icons/SimulateButton.png");
            m_IconStop = Texture2D::Create("Icons/StopButton.png");

            FramebufferParams fbParams;
            fbParams.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
            fbParams.Width = 1280;
            fbParams.Height = 720;
            m_Framebuffer = Framebuffer::Create(fbParams);

            m_EditorScene = NewRef<Scene>();
            m_ActiveScene = m_EditorScene;

            // TODO: Load scene from CLI
            /*auto commandLineArgs = Engine::Get().GetParams().CommandLineArgs;
            if (commandLineArgs.size() > 1) {
                auto scenePath = commandLineArgs[1];
                SceneSerializer serializer;
                m_ActiveScene = serializer.Deserialize(scenePath);
            }*/

            //Renderer2D::SetLineWidth(4.f);

            auto camera_e = m_ActiveScene->CreateEntity("Camera");
            camera_e.AddComponent<CameraComponent>();
            camera_e.GetComponent<TransformComponent>().SetTransform({-5, 0, 0}, { 0, 0, 0 }, {1, 1, 1});

            auto sprite_e = m_ActiveScene->CreateEntity("Sprite_1");
            sprite_e.AddComponent<PhysicsComponent>();
            sprite_e.AddComponent<SpriteComponent>(glm::vec4{ 0.8, 0.2, 0.3, 1.0 });
            sprite_e.GetComponent<TransformComponent>().SetTransform({ 0, 0, 0 }, {0, 0, 0}, {1, 1, 1});

            auto sprite_b = m_ActiveScene->CreateEntity("Sprite_2");
            sprite_b.AddComponent<PhysicsComponent>();
            sprite_b.AddComponent<SpriteComponent>(glm::vec4{ 0.3, 0.3, 0.9, 1.0 });
            sprite_b.GetComponent<TransformComponent>().SetTransform({ 0, 2, 0 }, { 0, 0, 0 }, { 1, 1, 1 });

            m_ScenePanel.SetContext(m_ActiveScene);
        }

        void EditorLayer::OnDetach() {
            VANTA_PROFILE_FUNCTION();
        }

        void EditorLayer::OnUpdate(double delta) {
            VANTA_PROFILE_FUNCTION();

            // Resize
            if (auto& params = m_Framebuffer->GetParams();
                m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
                (params.Width != m_ViewportSize.x || params.Height != m_ViewportSize.y))
            {
                m_Framebuffer->Resize((uint32)m_ViewportSize.x, (uint32)m_ViewportSize.y);
                //m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
                m_EditorCamera.Resize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
                m_ActiveScene->OnViewportResize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
            }

            constexpr float radius = 10.f;
            float camX = sin((float)Vanta::Duration::SinceLaunch().AsSecondsf()) * radius;
            float camY = cos((float)Vanta::Duration::SinceLaunch().AsSecondsf()) * radius;
            auto view = glm::lookAt(glm::vec3(camX, camY, 5), glm::vec3(0, 0, 0), glm::vec3(0.f, 1.f, 0.f));
            m_EditorCamera.SetTransform(glm::inverse(view));
            if (auto camera = m_ActiveScene->GetActiveCameraEntity()) {
                camera.GetComponent<TransformComponent>().SetTransform(glm::inverse(view));
            }

            // Render
            Renderer2D::ResetStats();
            m_Framebuffer->Bind();
            RenderCommand::Clear({ 0.1f, 0.1f, 0.1f, 1.f });

            // Clear our entity ID attachment to -1
            m_Framebuffer->ClearAttachment(1, -1);

            switch (m_State) {
            case State::Edit:
                if (m_ViewportFocused) {
                    //m_CameraController.OnUpdate(delta);
                }

                //m_EditorCamera.OnUpdate(delta);
                m_ActiveScene->OnUpdateEditor(delta, &m_EditorCamera);
                break;

            case State::Simulate:
                //m_EditorCamera.OnUpdate(delta);
                m_ActiveScene->OnUpdateSimulation(delta, &m_EditorCamera);
                break;

            case State::Play:
                m_ActiveScene->OnUpdateRuntime(delta);
                break;

            default:
                VANTA_UNREACHABLE("Invalid application state!");
                break;
            }

            auto [mx, my] = ImGui::GetMousePos();
            mx -= m_ViewportBounds[0].x;
            my -= m_ViewportBounds[0].y;
            glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
            my = viewportSize.y - my;
            int mouseX = (int)mx;
            int mouseY = (int)my;

            if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
                int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
                m_HoveredEntity = pixelData != -1 ? Entity((entt::entity)pixelData, m_ActiveScene.get()) : Entity();
            }

            RenderOverlay();

            m_Framebuffer->Unbind();
        }

        void EditorLayer::RenderOverlay() {
            if (m_State == State::Play) {
                Camera* camera = m_ActiveScene->GetActiveCamera();
                if (!camera)
                    return;

                Renderer2D::SceneBegin(camera);
            }
            else {
                Renderer2D::SceneBegin((Camera*) &m_EditorCamera);
            }

            /*if (m_ShowPhysicsColliders) {
                // Box Colliders
                {
                    auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
                    for (auto entity : view)
                    {
                        auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

                        glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
                        glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

                        glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                            * glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                            * glm::scale(glm::mat4(1.0f), scale);

                        Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
                    }
                }

                // Circle Colliders
                {
                    auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
                    for (auto entity : view)
                    {
                        auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

                        glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
                        glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

                        glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                            * glm::scale(glm::mat4(1.0f), scale);

                        Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
                    }
                }
            }*/

            // Draw selected entity outline
            /*if (Entity selectedEntity = m_ScenePanel.GetSelected()) {
                const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
                Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
            }*/

            Renderer2D::SceneEnd();
        }

        void EditorLayer::OnGUIRender() {
            VANTA_PROFILE_RENDER_FUNCTION();

            static bool dockspaceOpen = true;
            static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

            static bool isFullscreenPersistant = true;
            bool isFullscreen = isFullscreenPersistant;

            // Don't make the window itself a docking target
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (isFullscreen) {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
                windowFlags |=
                    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize   | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode,
            // DockSpace() will render our background and handle the pass-thru hole,
            // so we ask Begin() to not render a background.
            if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
                windowFlags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace", &dockspaceOpen, windowFlags);
            ImGui::PopStyleVar();

            if (isFullscreen)
                ImGui::PopStyleVar(2);

            // Dockspace
            ImGuiIO& io = ImGui::GetIO();
            ImGuiStyle& style = ImGui::GetStyle();
            float minSizeX = style.WindowMinSize.x;
            style.WindowMinSize.x = 370.0f;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                ImGuiID dockspaceID = ImGui::GetID("Vanta DockSpace");
                ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
            }

            style.WindowMinSize.x = minSizeX;

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    // Disabling fullscreen would allow the window to be moved to the front of other windows,
                    // which we can't undo at the moment without finer window depth/z control.
                    //ImGui::MenuItem("Fullscreen", NULL, &isFullscreenPersistant);1
                    if (ImGui::MenuItem("New", "Ctrl+N"))
                        NewScene();

                    if (ImGui::MenuItem("Open...", "Ctrl+O"))
                        OpenScene();

                    if (ImGui::MenuItem("Save", "Ctrl+S"))
                        SaveScene();

                    if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                        SaveSceneAs();

                    if (ImGui::MenuItem("Exit"))
                        Engine::Get().Stop();

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            Console::Get().OnGUIRender();
            m_ScenePanel.OnGUIRender();
            //m_ContentBrowserPanel.OnGUIRender();

            {
                ImGui::Begin("Stats");

                std::string hoveredName = "None";
                if (m_HoveredEntity)
                    hoveredName = m_HoveredEntity.GetComponent<IDComponent>().Name;

                ImGui::Text(FMT("Hovered Entity: {}", hoveredName).c_str());

                auto stats = Renderer2D::GetStats();
                ImGui::Text("Renderer2D Stats:");
                ImGui::Text(FMT("FPS: {}", Engine::Get().GetFPS()).c_str());
                ImGui::Text(FMT("Draw Calls: {}", stats.DrawCalls).c_str());
                ImGui::Text(FMT("Quads: {}", stats.QuadCount).c_str());
                ImGui::Text(FMT("Vertices: {}", stats.GetVertexCount()).c_str());
                ImGui::Text(FMT("Indices: {}", stats.GetIndexCount()).c_str());

                ImGui::End();
            }

            {
                ImGui::Begin("Settings");
                ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsColliders);
                ImGui::End();
            }

            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
                ImGui::Begin("Viewport");

                auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
                auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
                auto viewportOffset = ImGui::GetWindowPos();
                m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
                m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

                m_ViewportFocused = ImGui::IsWindowFocused();
                m_ViewportHovered = ImGui::IsWindowHovered();
                Engine::Get().GetGUILayer()->BlockEvents(m_ViewportFocused || m_ViewportHovered);

                ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
                m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

                uint64 textureID = m_Framebuffer->GetColorAttachmentRendererID();
                ImGui::Image(reinterpret_cast<ImTextureID>(textureID), viewportPanelSize, { 0, 1 }, { 1, 0 });

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                        const wchar_t* path = (const wchar_t*)payload->Data;
                        OpenScene(path);
                    }
                    ImGui::EndDragDropTarget();
                }

                // Gizmos
                /*Entity selectedEntity = m_ScenePanel.GetSelected();
                if (selectedEntity && m_GizmoType != -1) {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();

                    ImGuizmo::SetRect(
                        m_ViewportBounds[0].x,
                        m_ViewportBounds[0].y,
                        m_ViewportBounds[1].x - m_ViewportBounds[0].x,
                        m_ViewportBounds[1].y - m_ViewportBounds[0].y);

                    // Camera

                    // TODO: Runtime camera from entity
                    // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
                    // const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
                    // const glm::mat4& cameraProjection = camera.GetProjection();
                    // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

                    // Editor camera
                    const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
                    glm::mat4 cameraView = m_EditorCamera.GetView();

                    // Entity transform
                    auto& tc = selectedEntity.GetComponent<TransformComponent>();
                    glm::mat4 transform = tc.GetTransform();

                    // Snapping
                    bool snap = Input::IsKeyPressed(Key::LeftControl);
                    float snapValue = 0.5f; // Snap to 0.5m for translation/scale
                    // Snap to 45 degrees for rotation
                    if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                        snapValue = 45.0f;

                    float snapValues[3] = { snapValue, snapValue, snapValue };

                    ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                        (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                        nullptr, snap ? snapValues : nullptr);

                    if (ImGuizmo::IsUsing()) {
                        tc.SetTransform(transform);
                    }
                }*/

                ImGui::End();
                ImGui::PopStyleVar();
            }

            RenderToolbar();

            ImGui::End();
        }

        void EditorLayer::RenderToolbar() {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(128, 128));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

            auto& colors = ImGui::GetStyle().Colors;
            const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
            const auto& buttonActive = colors[ImGuiCol_ButtonActive];
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

            ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            bool toolbarEnabled = (bool)m_ActiveScene;

            ImVec4 tintColor = { 1, 1, 1, 1 };
            if (!toolbarEnabled)
                tintColor.w = 0.5f;

            auto windowSize = ImGui::GetContentRegionAvail();
            float size = std::min(windowSize.y, windowSize.x / 2);
            {
                Ref<Texture2D> icon = (m_State == State::Edit || m_State == State::Simulate) ? m_IconPlay : m_IconStop;
                usize texID = icon->GetRendererID();

                ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x * 0.5f) - size - 8);

                if (ImGui::ImageButton((ImTextureID)texID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
                    if (m_State == State::Edit || m_State == State::Simulate)
                        OnPlay();
                    else if (m_State == State::Play)
                        OnStop();
                }
            }
            ImGui::SameLine(0.f, 16.f);
            {
                Ref<Texture2D> icon = (m_State == State::Edit || m_State == State::Play) ? m_IconSimulate : m_IconStop;		//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
                usize texID = icon->GetRendererID();

                if (ImGui::ImageButton((ImTextureID)texID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
                    if (m_State == State::Edit || m_State == State::Play)
                        OnSimulate();
                    else if (m_State == State::Simulate)
                        OnStop();
                }
            }

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
            ImGui::End();
        }

        void EditorLayer::OnEvent(Event& e) {
            if (m_State == State::Edit) {
                //m_CameraController.OnEvent(e);
            }

            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<MouseButtonPressEvent>(EVENT_METHOD(EditorLayer::OnMouseButtonPress));
            dispatcher.Dispatch<KeyPressEvent>(EVENT_METHOD(EditorLayer::OnKeyPress));
        }

        bool EditorLayer::OnMouseButtonPress(MouseButtonPressEvent& e) {
            if (e.Button == Mouse::ButtonLeft) {
                if (m_ViewportHovered && !ImGuizmo::IsOver() && Input::IsKeyPressed(Key::LeftAlt)) {
                    m_ScenePanel.SetSelected(m_HoveredEntity);
                }
            }

            return false;
        }

        bool EditorLayer::OnKeyPress(KeyPressEvent& e) {
            // Shortcuts
            if (e.IsRepeat())
                return false;

            bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
            bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

            switch (e.Key) {
            case Key::N:
                if (control)
                    NewScene();
                break;

            case Key::O:
                if (control)
                    OpenScene();
                break;

            case Key::S:
                if (control) {
                    if (shift)
                        SaveSceneAs();
                    else
                        SaveScene();
                }
                break;

            // Scene Commands
            case Key::D:
                if (control)
                    OnDuplicateEntity();
                break;

            // Gizmos
            case Key::Q:
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = -1;
                break;

            case Key::W:
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;

            case Key::E:
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;

            case Key::R:
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;

            default: break;
            }

            return false;
        }

        void EditorLayer::OnPlay() {
            if (m_State == State::Simulate)
                OnStop();

            m_State = State::Play;

            //m_ActiveScene = Scene::Copy(m_EditorScene);
            m_ActiveScene->OnRuntimeBegin();

            m_ScenePanel.SetContext(m_ActiveScene);
        }

        void EditorLayer::OnSimulate() {
            if (m_State == State::Play)
                OnStop();

            m_State = State::Simulate;

            //m_ActiveScene = Scene::Copy(m_EditorScene);
            m_ActiveScene->OnSimulationBegin();

            m_ScenePanel.SetContext(m_ActiveScene);
        }

        void EditorLayer::OnStop() {
            VANTA_CORE_ASSERT(m_State == State::Play || m_State == State::Simulate, "");

            switch (m_State) {
            case State::Play:
                m_ActiveScene->OnRuntimeEnd();
                break;
            case State::Simulate:
                m_ActiveScene->OnSimulationEnd();
                break;
            default:
                break;
            }

            m_State = State::Edit;

            m_ActiveScene = m_EditorScene;

            m_ScenePanel.SetContext(m_ActiveScene);
        }

        void EditorLayer::OnDuplicateEntity() {
            if (m_State != State::Edit)
                return;

            VANTA_UNIMPLEMENTED();
            Entity selectedEntity = m_ScenePanel.GetSelected();
            if (selectedEntity)
                m_EditorScene->DuplicateEntity(selectedEntity);
        }

        void EditorLayer::NewScene() {
            m_ActiveScene = NewRef<Scene>();
            m_ActiveScene->OnViewportResize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);

            m_ScenePanel.SetContext(m_ActiveScene);

            //m_EditorScenePath = std::filesystem::path();
        }

        void EditorLayer::OpenScene() {
            VANTA_UNIMPLEMENTED();
            //std::string filepath = FileDialogs::OpenFile("Vanta Scene (*.hazel)\0*.hazel\0");
            //if (!filepath.empty())
            //    OpenScene(filepath);
        }

        void EditorLayer::OpenScene(const Path& path) {
            if (m_State != State::Edit)
                OnStop();

            if (path.extension().string() != ".vanta") {
                VANTA_WARN("Could not load {}; incorrect file type", path.filename().string());
                return;
            }

            VANTA_UNIMPLEMENTED();
            /*SceneSerializer serializer;
            Ref<Scene> newScene = serializer.Deserialize(path);
            if (newScene) {
                m_EditorScene = newScene;
                m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
                m_ScenePanel.SetContext(m_EditorScene);

                m_ActiveScene = m_EditorScene;
                m_EditorScenePath = path;
            }*/
        }

        void EditorLayer::SaveScene() {
            VANTA_UNIMPLEMENTED();
            //if (!m_EditorScenePath.empty()) {
            //    SceneSerializer serializer;
            //    serializer.Serialize(m_ActiveScene, m_EditorScenePath);
            //}
            //else {
            //    SaveSceneAs();
            //}
        }

        void EditorLayer::SaveSceneAs() {
            VANTA_UNIMPLEMENTED();
            //std::string filepath = FileDialogs::SaveFile("Vanta Scene (*.hazel)\0*.hazel\0");
            //if (!filepath.empty()) {
            //    SceneSerializer serializer;
            //    serializer.Serialize(m_ActiveScene, filepath);
            //    m_EditorScenePath = filepath;
            //}
        }
    }
}
