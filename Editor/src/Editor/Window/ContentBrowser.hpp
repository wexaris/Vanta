#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta {
    namespace Editor {

        class ContentBrowser {
        public:
            ContentBrowser();

            void OnGUIRender(bool allowInteraction = true);

        private:
            friend class EditorLayer;

            Path m_CurrentDirectory;

            Ref<Texture2D> m_FileIcon;
            Ref<Texture2D> m_FolderIcon;

            void OnClick(const std::filesystem::directory_entry& item);

            Ref<Texture2D> GetIcon(const std::filesystem::directory_entry& item) const;

            void OnWorkingDirectoryChange();
        };
    }
}
