#include "Editor/Window/SceneHierarchy.hpp"

#include <Vanta/Script/ScriptEngine.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace Vanta {
    namespace Editor {

        SceneHierarchy::SceneHierarchy(const Ref<Scene>& context) {
            SetContext(context);
        }

        void SceneHierarchy::SetContext(const Ref<Scene>& context) {
            m_Context = context;
            m_SelectedEntity = Entity();
        }

        void SceneHierarchy::OnGUIRender(bool allowInteraction) {
            ImGui::Begin("Scene");
            if (m_Context) {
                m_Context->GetRegistry().Each([&](auto entityHandle) {
                    Entity entity(entityHandle, m_Context.get());
                    DrawEntity(entity);
                });

                if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
                    m_SelectedEntity = Entity();

                if (allowInteraction) {
                    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
                        if (ImGui::MenuItem("Create New Entity"))
                            m_Context->CreateEntity("Entity");

                        ImGui::EndPopup();
                    }
                }
            }
            ImGui::End();

            ImGui::Begin("Properties");
            if (m_SelectedEntity) {
                DrawComponents(m_SelectedEntity);
            }
            ImGui::End();
        }

        void SceneHierarchy::DrawEntity(Entity entity) {
            auto name = entity.GetName();

            ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

            usize entityHandle = (usize)(uint32)entity;
            bool opened = ImGui::TreeNodeEx((void*)entityHandle, flags, name.c_str());

            if (ImGui::IsItemClicked()) {
                m_SelectedEntity = entity;
            }

            bool entityDeleted = false;
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Delete"))
                    entityDeleted = true;

                ImGui::EndPopup();
            }

            if (opened) {
                // TODO: Draw child entity items
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
                bool opened = ImGui::TreeNodeEx((void*)9817239, flags, name.c_str());
                if (opened)
                    ImGui::TreePop();
                ImGui::TreePop();
            }

            if (entityDeleted) {
                m_Context->DestroyEntity(entity);
                if (m_SelectedEntity == entity)
                    m_SelectedEntity = Entity();
            }
        }

        static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
            ImGuiIO& io = ImGui::GetIO();
            auto boldFont = io.Fonts->Fonts[0];

            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text(label.c_str());
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushFont(boldFont);
            if (ImGui::Button("X", buttonSize))
                values.x = resetValue;
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Y", buttonSize))
                values.y = resetValue;
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Z", buttonSize))
                values.z = resetValue;
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();

            ImGui::Columns(1);

            ImGui::PopID();
        }

        template<typename T, typename UIFunction>
        static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
            const ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_DefaultOpen |
                ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding |
                ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;

            if (entity.HasComponent<T>()) {
                ImVec2 contentRegion = ImGui::GetContentRegionAvail();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                ImGui::Separator();
                bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
                ImGui::PopStyleVar();

                ImGui::SameLine(contentRegion.x - lineHeight * 0.5f);
                if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
                    ImGui::OpenPopup("ComponentSettings");
                }

                bool removeComponent = false;
                if (ImGui::BeginPopup("ComponentSettings")) {
                    if (ImGui::MenuItem("Remove component"))
                        removeComponent = true;

                    ImGui::EndPopup();
                }

                if (open) {
                    uiFunction(entity.GetComponent<T>());
                    ImGui::TreePop();
                }

                if (removeComponent) {
                    entity.RemoveComponent<T>();
                }
            }
        }

        void SceneHierarchy::DrawComponents(Entity entity) {
            auto& name = entity.GetComponent<IDComponent>().Name;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, name.c_str(), name.length());
            if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                name = std::string(buffer);
            }
            
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);

            if (ImGui::Button("Add Component"))
                ImGui::OpenPopup("AddComponent");

            if (ImGui::BeginPopup("AddComponent")) {
                DrawAddComponentMenu<CameraComponent>("Camera");
                DrawAddComponentMenu<ScriptComponent>("Script");
                DrawAddComponentMenu<SpriteComponent>("Sprite Renderer");
                DrawAddComponentMenu<CircleRendererComponent>("Circle Renderer");
                DrawAddComponentMenu<Rigidbody2DComponent>("Rigidbody 2D"); 
                DrawAddComponentMenu<BoxCollider2DComponent>("Box Collider 2D");
                DrawAddComponentMenu<CircleCollider2DComponent>("Circle Collider 2D");
                ImGui::EndPopup();
            }

            ImGui::PopItemWidth();

            DrawComponent<TransformComponent>("Transform", entity, [](auto& buffer) {
                auto& tr = buffer.Get();

                glm::vec3 position = tr.GetPosition();
                glm::vec3 rotation = tr.GetRotationDegrees();
                glm::vec3 scale = tr.GetScale();

                DrawVec3Control("Position", position);
                DrawVec3Control("Rotation", rotation);
                DrawVec3Control("Scale", scale, 1.0f);

                if (position != tr.GetPosition() ||
                    rotation != tr.GetRotationDegrees() ||
                    scale != tr.GetScale())
                {
                    buffer.Set().SetTransformDeg(position, rotation, scale);
                }
            });

            DrawComponent<CameraComponent>("Camera", entity, [&](auto& component) {
                SceneCamera& camera = component.Camera;

                if (ImGui::Button("Make Primary")) {
                    m_Context->SetActiveCameraEntity(entity);
                }

                const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
                if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
                    for (int i = 0; i < 2; i++) {
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
                            currentProjectionTypeString = projectionTypeStrings[i];
                            camera.SetProjectionType((SceneCamera::Projection)i);
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (camera.GetProjectionType() == SceneCamera::Projection::Perspective) {
                    float perspectiveFov = camera.GetPerspectiveFOV();
                    if (ImGui::DragFloat("Vertical FOV", &perspectiveFov))
                        camera.SetPerspectiveFOV(perspectiveFov);

                    float perspectiveNear = camera.GetPerspectiveNearClip();
                    if (ImGui::DragFloat("Near", &perspectiveNear))
                        camera.SetPerspectiveNearClip(perspectiveNear);

                    float perspectiveFar = camera.GetPerspectiveFarClip();
                    if (ImGui::DragFloat("Far", &perspectiveFar))
                        camera.SetPerspectiveFarClip(perspectiveFar);
                }

                if (camera.GetProjectionType() == SceneCamera::Projection::Orthographic) {
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat("Size", &orthoSize))
                        camera.SetOrthographicSize(orthoSize);

                    float orthoNear = camera.GetOrthographicNearClip();
                    if (ImGui::DragFloat("Near", &orthoNear))
                        camera.SetOrthographicNearClip(orthoNear);

                    float orthoFar = camera.GetOrthographicFarClip();
                    if (ImGui::DragFloat("Far", &orthoFar))
                        camera.SetOrthographicFarClip(orthoFar);

                    ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
                }
            });

            DrawComponent<ScriptComponent>("Circle", entity, [](ScriptComponent& component) {
                static char buffer[64];
                strcpy_s(buffer, component.ClassName.c_str());

                bool classExists = ScriptEngine::EntityClassExists(component.ClassName);

                if (!classExists)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

                if (ImGui::InputText("Class", buffer, sizeof(buffer)))
                    component.ClassName = buffer;

                if (!classExists)
                    ImGui::PopStyleColor();
            });

            DrawComponent<SpriteComponent>("Sprite", entity, [](SpriteComponent& component) {
                ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

                if (component.Texture) {
                    usize texID = component.Texture->GetRendererID();
                    ImGui::ImageButton((ImTextureID)texID, ImVec2(100.f, 100.f));

                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::MenuItem("Remove"))
                            component.Texture = nullptr;
                        ImGui::EndPopup();
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const wchar_t* path = (const wchar_t*)payload->Data;
                            component.Texture = Texture2D::Create(path);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                else {
                    ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const wchar_t* path = (const wchar_t*)payload->Data;
                            component.Texture = Texture2D::Create(path);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
            });

            DrawComponent<CircleRendererComponent>("Circle", entity, [](auto& component) {
                ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
                ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
                ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
            });

            DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component) {
                const char* optionStrings[] = { "Static", "Dynamic", "Kinematic" };
                const char* currentOptionString = optionStrings[(int)component.Type];

                if (ImGui::BeginCombo("Type", currentOptionString)) {
                    for (int i = 0; i < 3; i++) {
                        bool isSelected = currentOptionString == optionStrings[i];
                        if (ImGui::Selectable(optionStrings[i], isSelected)) {
                            currentOptionString = optionStrings[i];
                            component.Type = (Rigidbody2DComponent::BodyType)i;
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
            });

            DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component) {
                ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
                ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
                ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
            });

            DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component) {
                ImGui::DragFloat("Radius", &component.Radius);
                ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
                ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
            });
        }

        template<typename T>
        void SceneHierarchy::DrawAddComponentMenu(const std::string& entityName) {
            if (!m_SelectedEntity.HasComponent<T>()) {
                if (ImGui::MenuItem(entityName.c_str())) {
                    m_SelectedEntity.AddComponent<T>();
                    ImGui::CloseCurrentPopup();
                }
            }
        }
    }
}
