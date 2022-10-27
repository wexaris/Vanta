#include "Editor/EditorLayer.hpp"

#define VANTA_CUSTOM_LOG_SINKS
#include <Vanta/EntryPoint.hpp>

namespace Vanta {
    namespace Editor {

        class Editor : public Engine {
        public:
            Editor(const EngineParams& params) : Engine(params) {
                PushLayer(new EditorLayer());
            }
        };
    }

    void CreateLogSinks(Log::SinkList& sinks) {
        auto console_sink = NewRef<Editor::ConsoleSink_mt>();
        sinks.push_back(console_sink);
    }

    Engine* CreateEngine(CommandLineArguments args) {
        EngineParams params;
        params.CommandLineArgs = args;
        params.Window.Title = "Vanta Editor";
        params.Window.Width = 1600;
        params.Window.Height = 1000;
        params.Window.VerticalSync = false;
        params.Window.IconPath = "Icons/Vanta.png";
        return new Editor::Editor(params);
    }
}
