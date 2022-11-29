#include "Editor/Window/ContentBrowser.hpp"

#include <imgui.h>

namespace Vanta {
    namespace Editor {

        ContentBrowser::ContentBrowser()
            : m_CurrentDirectory(Engine::Get().AssetDirectory())
        {
            m_FileIcon = Texture2D::Create(Engine::RuntimeResourceDirectory() / "Icons/ContentBrowser/File.png");
            m_FolderIcon = Texture2D::Create(Engine::RuntimeResourceDirectory() / "Icons/ContentBrowser/Folder.png");
        }

        void ContentBrowser::OnGUIRender(bool allowInteraction) {
            ImGui::Begin("Content Browser");
            
            if (m_CurrentDirectory != Engine::Get().AssetDirectory()) {
                if (ImGui::ArrowButton("Up", ImGuiDir_Left)) {
                    m_CurrentDirectory = m_CurrentDirectory.parent_path();
                }
            }

            static float padding = 12.0f;
            static float thumbnailSize = 64.0f;
            float cellSize = thumbnailSize + padding;

            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = (int)(panelWidth / cellSize);
            if (columnCount < 1)
                columnCount = 1;

            ImGui::Columns(columnCount, 0, false);

			for (auto& item : std::filesystem::directory_iterator(m_CurrentDirectory)) {
				const auto& path = item.path();
				std::string filenameStr = path.filename().string();

                Ref<Texture2D> icon = GetIcon(item);
                usize texID = icon->GetRendererID();

				ImGui::PushID(filenameStr.c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)texID, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				
                if (ImGui::BeginDragDropSource()) {
					auto relativePath = std::filesystem::relative(path, Engine::Get().AssetDirectory());
					const wchar_t* itemPath = relativePath.c_str();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
					ImGui::EndDragDropSource();
				}

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    OnClick(item);
                }

				ImGui::TextWrapped(filenameStr.c_str());

                ImGui::PopStyleColor();
                ImGui::PopID();

				ImGui::NextColumn();
			}

			ImGui::Columns(1);

			ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
			ImGui::SliderFloat("Padding", &padding, 0, 32);

            ImGui::End();
        }

        void ContentBrowser::OnClick(const std::filesystem::directory_entry& item) {
            if (item.is_directory()) {
                m_CurrentDirectory /= item.path().filename();
            }
            else {
                if (item.path().extension() == "vnta") {
                    
                }
            }
        }

        Ref<Texture2D> ContentBrowser::GetIcon(const std::filesystem::directory_entry& item) const {
            return item.is_directory() ? m_FolderIcon : m_FileIcon;
        }

        void ContentBrowser::OnWorkingDirectoryChange() {
            m_CurrentDirectory = Engine::Get().AssetDirectory();
        }
    }
}
