#include "Editor/EditorLayer.hpp"

namespace Vanta {
    namespace Editor {

        class Editor : public Engine {
        public:
            Editor(const EngineParams& params) : Engine(params) {
                PushLayer(new EditorLayer());
            }
        };
    }

    Engine* CreateEngine() {
        EngineParams params;
        params.Window.Title = "Vanta Editor";
        return new Editor::Editor(params);
    }
}
