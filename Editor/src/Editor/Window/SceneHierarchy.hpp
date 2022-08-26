#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta {
    namespace Editor {

        class SceneHierarchy {
        public:
            SceneHierarchy() = default;
            SceneHierarchy(const Ref<Scene>& context);

            void SetContext(const Ref<Scene>& context);

            void OnGUIRender(bool allowInteraction = true);
            
            void SetSelected(Entity entity) { m_SelectedEntity = entity;  }
            Entity GetSelected() const      { return m_SelectedEntity; }

        private:
            Ref<Scene> m_Context;
            Entity m_SelectedEntity;

            void DrawEntity(Entity entity);
            void DrawComponents(Entity entity);

            template<typename T>
            void DrawAddComponentMenu(const std::string& entityName);
        };
    }
}
