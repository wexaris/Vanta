#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta {
    namespace Editor {

        class ContentBrowser {
        public:
            ContentBrowser();

            void OnGUIRender(bool allowInteraction = true);

        private:
            Path m_CurrentDirectory;

            Ref<Texture2D> m_FileIcon;
            Ref<Texture2D> m_FolderIcon;

            Ref<Texture2D> GetIcon(const std::filesystem::directory_entry& item) const;

            void OnClick(const std::filesystem::directory_entry& item);
        };
    }
}
