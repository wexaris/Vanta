#include "Editor/EditorLayer.hpp"
#include "Editor/Panel/Console.hpp"

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
        //console_sink->set_pattern("[%T] [%1] %n: %v");
        sinks.push_back(console_sink);
    }

    Engine* CreateEngine() {
        EngineParams params;
        params.Window.Title = "Vanta Editor";
        return new Editor::Editor(params);
    }
}
