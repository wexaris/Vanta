#pragma once
#include "Vanta/Core/GUILayer.hpp"
#include "Vanta/Core/LayerStack.hpp"
#include "Vanta/Core/Window.hpp"
#include "Vanta/Event/KeyEvent.hpp"
#include "Vanta/Event/MouseEvent.hpp"
#include "Vanta/Event/WindowEvent.hpp"
#include "Vanta/Render/Camera.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"

namespace Vanta {
    struct CommandLineArguments {
        usize Count = 0;
        char** Args = nullptr;

        CommandLineArguments() = default;
        CommandLineArguments(usize argc, char** argv)
            : Count(argc), Args(argv) {}

        const char* operator[](usize idx) const {
            VANTA_CORE_ASSERT(idx < Count, "Invalid command line argument index: {} of {}", idx + 1, Count);
            return Args[idx];
        }
    };

    struct EngineParams {
        CommandLineArguments CommandLineArgs;
        Path WorkingDirectory = std::filesystem::current_path();
        WindowParams Window;
    };

    class Engine {
    public:
        Engine(const EngineParams& params = EngineParams());
        virtual ~Engine();

        void Run();
        void Stop();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        uint GetFPS() const         { return (uint)(1.0 / m_DeltaTime + 0.5); }
        double GetDeltaTime() const { return m_DeltaTime; }

        const CommandLineArguments& GetCommandLineArgs() const { return m_CommandLineArgs; }

        static Path RuntimeDirectory()         { return s_RuntimeDirectory; }
        static Path RuntimeResourceDirectory() { return s_RuntimeDirectory / "Resources"; }

        void SetWorkingDirectory(const Path& cwd) { m_WorkingDirectory = cwd; }

        Path WorkingDirectory() const  { return m_WorkingDirectory; }
        Path AssetDirectory() const    { return m_WorkingDirectory / "Assets"; }
        Path ScriptDirectory() const   { return m_WorkingDirectory / "Scripts"; }
        Path CacheDirectory() const    { return m_WorkingDirectory / "Cache"; }
        Path CorrectFilepath(const Path& path);

        bool IsMinimized() const { return m_Minimized; }

        static Engine& Get()      { return *s_Instance; }
        Window& GetWindow()       { return *m_Window; }
        GUILayer* GetGUILayer()   { return m_GUILayer; }

        void SubmitToMainThread(const std::function<void()>& func);

    private:
        static Engine* s_Instance;

        Box<Window> m_Window;
        LayerStack m_LayerStack;
        GUILayer* m_GUILayer;

        Path m_WorkingDirectory;
        CommandLineArguments m_CommandLineArgs;
        static const Path s_RuntimeDirectory;

        double m_DeltaTime;
        bool m_Running = true;
        bool m_Minimized = false;

        std::vector<std::function<void()>> m_MainThreadQueue;
        std::mutex m_MainThreadQueueMutex;

        void OnEvent(Event& e);
        virtual bool OnWindowClose(WindowCloseEvent& e);
        virtual bool OnWindowMove(WindowMoveEvent& e);
        virtual bool OnWindowResize(WindowResizeEvent& e);
        virtual bool OnWindowMaximize(WindowMaximizeEvent& e);
        virtual bool OnWindowMinimize(WindowMinimizeEvent& e);
        virtual bool OnWindowRestore(WindowRestoreEvent& e);
        virtual bool OnWindowGainFocus(WindowGainFocusEvent& e);
        virtual bool OnWindowLoseFocus(WindowLoseFocusEvent& e);

        void ExectuteMainThreadQueue();
    };
}
