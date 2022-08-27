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
    struct EngineParams {
        uint MinTickRate = 60;
        uint MaxSubsteps = 4;
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

        static Path RuntimeDirectory() { return s_RuntimeDirectory; }
        Path WorkingDirectory() const  { return m_WorkingDirectory; }
        Path AssetDirectory() const    { return m_WorkingDirectory / "Assets"; }
        Path SourceDirectory() const   { return m_WorkingDirectory / "Source"; }
        Path CorrectFilepath(const Path& path);

        bool IsMinimized() const { return m_Minimized; }

        static Engine& Get()      { return *s_Instance; }
        Window& GetWindow()       { return *m_Window; }
        GUILayer* GetGUILayer()   { return m_GUILayer; }

    protected:
        static const Path s_RuntimeDirectory;

        Box<Window> m_Window;
        LayerStack m_LayerStack;
        GUILayer* m_GUILayer;

        uint m_MinTickRate;
        uint m_MaxSubsteps;
        Path m_WorkingDirectory;

        double m_DeltaTime;
        bool m_Running = true;
        bool m_Minimized = false;

        void OnEvent(Event& e);
        virtual bool OnWindowClose(WindowCloseEvent& e);
        virtual bool OnWindowMove(WindowMoveEvent& e);
        virtual bool OnWindowResize(WindowResizeEvent& e);
        virtual bool OnWindowMaximize(WindowMaximizeEvent& e);
        virtual bool OnWindowMinimize(WindowMinimizeEvent& e);
        virtual bool OnWindowRestore(WindowRestoreEvent& e);
        virtual bool OnWindowGainFocus(WindowGainFocusEvent& e);
        virtual bool OnWindowLoseFocus(WindowLoseFocusEvent& e);

    private:
        static Engine* s_Instance;
    };
}
