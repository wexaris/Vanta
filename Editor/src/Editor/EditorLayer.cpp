#include "Editor/EditorLayer.hpp"

#include <Vanta/Project/Project.hpp>
#include <Vanta/Scene/Serializer.hpp>
#include <Vanta/Script/ScriptEngine.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

namespace Vanta {
    namespace Editor {

        EditorLayer::EditorLayer() :
            Layer("Editor Layer") {}

        void EditorLayer::OnAttach() {
            VANTA_PROFILE_FUNCTION();

            m_IconPlay = Texture2D::Create(Engine::RuntimeResourceDirectory() / "Icons/PlayButton.png");
            m_IconSimulate = Texture2D::Create(Engine::RuntimeResourceDirectory() / "Icons/SimulateButton.png");
            m_IconPause = Texture2D::Create(Engine::RuntimeResourceDirectory() / "Icons/PauseButton.png");
            m_IconStep = Texture2D::Create(Engine::RuntimeResourceDirectory() / "Icons/StepButton.png");
            m_IconStop = Texture2D::Create(Engine::RuntimeResourceDirectory() / "Icons/StopButton.png");

            FramebufferParams fbParams;
            fbParams.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
            fbParams.Width = 1280;
            fbParams.Height = 720;
            m_Framebuffer = Framebuffer::Create(fbParams);

            auto& commandLineArgs = Engine::Get().GetCommandLineArgs();
            if (commandLineArgs.Count > 1) {
                auto projectPath = commandLineArgs[1];
                OpenProject(projectPath);
            }
            else {
                OpenProject();
            }
        }

        void EditorLayer::OnDetach() {
            VANTA_PROFILE_FUNCTION();
        }

        void EditorLayer::OnUpdate(double delta) {
            VANTA_PROFILE_FUNCTION();

            // Handle resize
            if (auto& params = m_Framebuffer->GetParams();
                m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
                (params.Width != m_ViewportSize.x || params.Height != m_ViewportSize.y))
            {
                m_Framebuffer->Resize((uint32)m_ViewportSize.x, (uint32)m_ViewportSize.y);
                m_EditorCamera.OnViewportResize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
                m_ActiveScene->OnViewportResize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
            }

            // Render
            Renderer2D::ResetStats();
            m_Framebuffer->Bind();
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
            RenderCommand::Clear();

            // Clear our entity ID attachment to -1
            m_Framebuffer->ClearAttachment(1, -1);

            switch (m_State) {
            case State::Edit:
                if (m_ViewportFocused)
                    m_EditorCamera.OnUpdate(delta);

                m_ActiveScene->OnUpdateEditor(delta, &m_EditorCamera.GetCamera());
                break;

            case State::Simulate:
                if (m_ViewportFocused)
                    m_EditorCamera.OnUpdate(delta);

                m_ActiveScene->OnUpdateSimulation(delta, &m_EditorCamera.GetCamera());
                break;

            case State::Play:
                m_ActiveScene->OnUpdateRuntime(delta);
                break;

            default:
                VANTA_UNREACHABLE("Invalid application state!");
                break;
            }

            // Handle mouse over entity
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
            VANTA_PROFILE_RENDER_FUNCTION();

            if (m_State == State::Play) {
                Camera* camera = m_ActiveScene->GetActiveCamera();
                if (!camera)
                    return;

                Renderer2D::SceneBegin(camera);
            }
            else {
                Renderer2D::SceneBegin((Camera*) &m_EditorCamera);
            }

            if (m_ShowPhysicsColliders) {
                // Box Colliders
                m_ActiveScene->View<TransformComponent, BoxCollider2DComponent>(
                    [](auto, TransformComponent& tr, BoxCollider2DComponent& bc)
                {
                    glm::vec3 translation = tr.GetPosition() + glm::vec3(bc.Offset, 0.001f);
                    glm::vec3 scale = tr.GetScale() * glm::vec3(bc.Size * 2.0f, 1.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                        * glm::rotate(glm::mat4(1.0f), tr.GetRotationRadians().z, glm::vec3(0.0f, 0.0f, 1.0f))
                        * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
                });

                // Circle Colliders
                m_ActiveScene->View<TransformComponent, CircleCollider2DComponent>(
                    [](auto, TransformComponent& tr, CircleCollider2DComponent& cc)
                {
                    glm::vec3 translation = tr.GetPosition() + glm::vec3(cc.Offset, 0.001f);
                    glm::vec3 scale = tr.GetScale() * glm::vec3(cc.Radius * 2.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                        * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
                });
            }

            // Draw selected entity outline
            if (Entity selectedEntity = m_ScenePanel.GetSelected()) {
                TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
                Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.9f, 0.2f, 1.0f));
            }

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
                        NewProject();

                    if (ImGui::MenuItem("Save", "Ctrl+S"))
                        SaveProject();

                    if (ImGui::MenuItem("Open...", "Ctrl+O"))
                        OpenProject();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Exit"))
                        Engine::Get().Stop();

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Scripts")) {
                    if (ImGui::MenuItem("Reload Assembly"))
                        ScriptEngine::ReloadAssembly();

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            Console::Get().OnGUIRender(!m_ViewportActive);
            m_ScenePanel.OnGUIRender(!m_ViewportActive);
            if (m_ContentPanel)
                m_ContentPanel->OnGUIRender(!m_ViewportActive);

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

                m_ViewportHovered = ImGui::IsWindowHovered();
                m_ViewportFocused = ImGui::IsWindowFocused();
                m_ViewportActive = m_EditorCamera.IsActive() || m_ViewportHovered;
                Engine::Get().GetGUILayer()->BlockEvents(!m_ViewportActive);

                if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    ImGui::SetWindowFocus();
                    auto event = MouseButtonPressEvent(Mouse::ButtonLeft);
                    OnEvent(event);
                }
                if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    ImGui::SetWindowFocus();
                    auto event = MouseButtonPressEvent(Mouse::ButtonRight);
                    OnEvent(event);
                }

                if (m_State == State::Edit || m_State == State::Simulate) {
                    auto& window = Engine::Get().GetWindow();
                    if (m_EditorCamera.IsActive() && window.GetCursorMode() != CursorMode::Disabled)
                        window.SetCursorMode(CursorMode::Disabled);
                    else if (!m_EditorCamera.IsActive() && window.GetCursorMode() != CursorMode::Normal)
                        window.SetCursorMode(CursorMode::Normal);
                }

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
                Entity selectedEntity = m_ScenePanel.GetSelected();
                if (m_State != State::Play && selectedEntity && m_GizmoType != -1) {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();

                    ImGuizmo::SetRect(
                        m_ViewportBounds[0].x,
                        m_ViewportBounds[0].y,
                        m_ViewportBounds[1].x - m_ViewportBounds[0].x,
                        m_ViewportBounds[1].y - m_ViewportBounds[0].y);

                    // Get current camera
                    Camera& camera = m_EditorCamera.GetCamera();
                    const glm::mat4& cameraProjection = camera.GetProjection();
                    const glm::mat4& cameraView = camera.GetView();

                    // Entity transform
                    auto& tc = selectedEntity.GetComponent<TransformComponent>();
                    glm::mat4 transform = tc.Get().GetTransform();

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
                        tc.Set().SetTransform(transform);
                    }
                }

                ImGui::End();
                ImGui::PopStyleVar();
            }

            RenderToolbar();

            ImGui::End();
        }

        void EditorLayer::RenderToolbar() {
            VANTA_PROFILE_RENDER_FUNCTION();

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

            if (m_State == State::Edit) {
                ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x * 0.5f) - size - 8);

                {
                    auto& icon = m_IconPlay;
                    usize texID = icon->GetRendererID();

                    if (ImGui::ImageButton((ImTextureID)texID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
                        if (m_State == State::Edit || m_State == State::Simulate)
                            OnPlay();
                        else if (m_State == State::Play)
                            OnStop();
                    }
                }
                ImGui::SameLine(0.f, 16.f);
                {
                    auto& icon = m_IconSimulate;
                    usize texID = icon->GetRendererID();

                    if (ImGui::ImageButton((ImTextureID)texID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
                        if (m_State == State::Edit || m_State == State::Play)
                            OnSimulate();
                        else if (m_State == State::Simulate)
                            OnStop();
                    }
                }
            }
            else {
                ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x * 0.5f) - (size * 1.5f) - 8);

                {
                    auto& icon = m_IconStop;
                    usize texID = icon->GetRendererID();

                    if (ImGui::ImageButton((ImTextureID)texID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
                        OnStop();
                    }
                }
                ImGui::SameLine(0.f, 16.f);
                {
                    bool isPaused = m_ActiveScene->IsPaused();

                    auto& icon = isPaused ? m_IconPlay : m_IconPause;
                    usize texID = icon->GetRendererID();

                    if (ImGui::ImageButton((ImTextureID)texID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
                        m_ActiveScene->SetPaused(!isPaused);
                    }
                }
                ImGui::SameLine(0.f, 16.f);
                {
                    auto& icon = m_IconStep;
                    usize texID = icon->GetRendererID();

                    if (ImGui::ImageButton((ImTextureID)texID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
                        m_ActiveScene->Step(1);
                    }
                }
                
            }
            

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
            ImGui::End();
        }

        void EditorLayer::OnEvent(Event& e) {
            VANTA_PROFILE_FUNCTION();

            if (m_State == State::Edit || m_State == State::Simulate) {
                m_EditorCamera.OnEvent(e);
            }

            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<MouseButtonPressEvent>(EVENT_METHOD(EditorLayer::OnMouseButtonPress));
            dispatcher.Dispatch<KeyPressEvent>(EVENT_METHOD(EditorLayer::OnKeyPress));
        }

        bool EditorLayer::OnMouseButtonPress(MouseButtonPressEvent& e) {
            if (e.Button == Mouse::ButtonLeft) {
                if (m_State == State::Edit || m_State == State::Simulate) {
                    if (m_ViewportHovered) {
                        if ((m_HoveredEntity && m_HoveredEntity != m_ScenePanel.GetSelected()) || !ImGuizmo::IsOver()) {
                            m_ScenePanel.SetSelected(m_HoveredEntity);
                        }
                    }
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
            case Key::Escape:
                if (m_State != State::Edit)
                    OnStop();
                break;

            case Key::N:
                if (!m_EditorCamera.IsActive()) {
                    if (control)
                        NewProject();
                }
                break;

            case Key::O:
                if (!m_EditorCamera.IsActive()) {
                    if (control)
                        OpenProject();
                }
                break;

            case Key::S:
                if (!m_EditorCamera.IsActive()) {
                    if (control)
                        SaveProject();
                }
                break;

            // Scene Commands
            case Key::D:
                if (!m_EditorCamera.IsActive()) {
                    if (control)
                        OnDuplicateEntity();
                }
                break;

            // Gizmos
            case Key::Q:
                if (!m_EditorCamera.IsActive() && !ImGuizmo::IsUsing())
                    m_GizmoType = -1;
                break;

            case Key::W:
                if (!m_EditorCamera.IsActive() && !ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;

            case Key::E:
                if (!m_EditorCamera.IsActive() && !ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;

            case Key::R:
                if (!m_EditorCamera.IsActive() && !ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;

            default: break;
            }

            return false;
        }

        void EditorLayer::OnPlay() {
            VANTA_PROFILE_FUNCTION();

            if (m_State == State::Simulate)
                OnStop();

            m_State = State::Play;

            m_ActiveScene = Scene::Copy(m_EditorScene);
            m_ActiveScene->OnRuntimeBegin();

            m_ScenePanel.SetContext(m_ActiveScene);
        }

        void EditorLayer::OnSimulate() {
            VANTA_PROFILE_FUNCTION();

            if (m_State == State::Play)
                OnStop();

            m_State = State::Simulate;

            m_ActiveScene = Scene::Copy(m_EditorScene);
            m_ActiveScene->OnSimulationBegin();

            m_ScenePanel.SetContext(m_ActiveScene);
        }

        void EditorLayer::OnPause() {
            VANTA_PROFILE_FUNCTION();

            if (m_State == State::Edit)
                return;

            m_ActiveScene->SetPaused(true);
        }

        void EditorLayer::OnStop() {
            VANTA_PROFILE_FUNCTION();

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
            VANTA_PROFILE_FUNCTION();

            if (m_State != State::Edit)
                return;

            Entity selectedEntity = m_ScenePanel.GetSelected();
            if (selectedEntity)
                m_EditorScene->DuplicateEntity(selectedEntity);
        }

        void EditorLayer::NewProject() {
            VANTA_PROFILE_FUNCTION();

            if (m_State != State::Edit)
                OnStop();

            auto file = IO::FileDialog::OpenFile("Vanta Project (*.vproj)\0*.vproj\0");
            if (!file) {
                Engine::Get().Stop();
                return;
            }
                
            Ref<Project> project = Project::New(file.value());

            OpenScene(project->GetConfig().InitialScenePath);

            m_ContentPanel = NewBox<ContentBrowser>();
        }

        void EditorLayer::OpenProject() {
            VANTA_PROFILE_FUNCTION();

            // TODO: Prompt 
            auto file = IO::FileDialog::OpenFile("Vanta Project (*.vproj)\0*.vproj\0");
            if (file)
                OpenProject(file->Filepath);
        }

        void EditorLayer::OpenProject(const Path& filepath) {
            VANTA_PROFILE_FUNCTION();

            if (m_State != State::Edit)
                OnStop();

            if (filepath.extension().string() != ".vproj") {
                VANTA_ERROR("Could not load project: {}; incorrect file type", filepath.filename());
                return;
            }

            Ref<Project> project = Project::Load(filepath);
            if (!project) {
                VANTA_ERROR("Failed to load project: {}", filepath);
                return;
            }

            OpenScene(project->GetConfig().InitialScenePath);

            m_ContentPanel = NewBox<ContentBrowser>();
        }

        void EditorLayer::SaveProject() {
            VANTA_PROFILE_FUNCTION();
            SaveScene();
            Project::Save();
        }

        void EditorLayer::NewScene() {
            VANTA_PROFILE_FUNCTION();

            if (m_State != State::Edit)
                OnStop();

            m_EditorScene = NewRef<Scene>();

            m_ActiveScene = m_EditorScene;
            m_ScenePanel.SetContext(m_ActiveScene);

            m_SceneFilepath = Path();
        }

        void EditorLayer::OpenScene(const Path& filepath) {
            VANTA_PROFILE_FUNCTION();

            Path fixedFilepath = Project::GetAssetPath(filepath);

            if (m_State != State::Edit)
                OnStop();

            if (fixedFilepath.extension().string() != ".vnta") {
                VANTA_ERROR("Could not load {}; incorrect file type", filepath.filename());
                return;
            }

            // Deserialize scene
            Ref<Scene> newScene = NewRef<Scene>();
            SceneSerializer serializer(fixedFilepath);
            if (!serializer.Deserialize(newScene)) {
                VANTA_ERROR("Failed to parse scene file: {}", fixedFilepath.filename());
                return;
            }

            auto viewportCameraPos = serializer.Get<glm::vec3>("ViewportCameraPosition");
            if (viewportCameraPos) {
                m_EditorCamera.SetPosition(viewportCameraPos.value());
            }

            auto viewportCameraRot = serializer.Get<glm::vec3>("ViewportCameraRotation");
            if (viewportCameraRot) {
                m_EditorCamera.SetRotationDeg(viewportCameraRot.value());
            }

            // Setup scene data
            m_EditorScene = newScene;

            m_ActiveScene = m_EditorScene;
            m_ScenePanel.SetContext(m_ActiveScene);

            m_SceneFilepath = fixedFilepath;
        }

        void EditorLayer::SaveScene() {
            VANTA_PROFILE_FUNCTION();

            if (!m_SceneFilepath.empty()) {
                SceneSerializer serializer(m_SceneFilepath);
                serializer.Serialize(m_EditorScene);
                serializer.Append("ViewportCameraPosition", m_EditorCamera.GetPosition());
                serializer.Append("ViewportCameraRotation", m_EditorCamera.GetRotationDeg());
            }
            else {
                SaveSceneAs();
            }
        }

        void EditorLayer::SaveSceneAs() {
            VANTA_PROFILE_FUNCTION();

            auto file = IO::FileDialog::SaveFile("Vanta Scene (*.vnta)\0*.vnta\0");
            if (file) {
                SceneSerializer serializer(file.value());
                serializer.Serialize(m_EditorScene);
                serializer.Append("ViewportCameraPosition", m_EditorCamera.GetPosition());
                serializer.Append("ViewportCameraRotation", m_EditorCamera.GetRotationDeg());
                m_SceneFilepath = file->Filepath;
            }
        }
    }
}
