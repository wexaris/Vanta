#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Input/Input.hpp"
#include "Vanta/Render/Renderer.hpp"
#include "Vanta/Scene/Scene.hpp"
#include "Vanta/Scripts/ScriptEngine.hpp"

namespace Vanta {

    const Path Engine::s_RuntimeDirectory = std::filesystem::current_path();

    Engine* Engine::s_Instance = nullptr;

    Engine::Engine(const EngineParams& params) :
        m_CommandLineArgs(params.CommandLineArgs),
        m_WorkingDirectory(params.WorkingDirectory)
    {
        VANTA_PROFILE_FUNCTION();

        VANTA_CORE_ASSERT(!s_Instance, "Another Engine instance already exists!");
        s_Instance = this;

        // Set working directory
        if (!params.WorkingDirectory.empty())
            std::filesystem::current_path(params.WorkingDirectory);

        m_Window = Window::Create(params.Window);
        m_Window->SetEventCallback(EVENT_METHOD(Engine::OnEvent));

        Fibers::Init();
        Renderer::Init();
        ScriptEngine::Init();

        m_GUILayer = new GUILayer();
        PushOverlay(m_GUILayer);
    }

    Engine::~Engine() {
        VANTA_PROFILE_FUNCTION();
        ScriptEngine::Shutdown();
        Renderer::Shutdown();
        Fibers::Shutdown();
    }

    void Engine::Run() {
        VANTA_PROFILE_FUNCTION();

        Time lastFrameTime;

        while (m_Running) {
            VANTA_PROFILE_SCOPE("Game Loop");

            ExectuteMainThreadQueue();

            Input::PollInputs();

            for (Layer* layer : m_LayerStack) {
                layer->OnUpdate(m_DeltaTime);
            }

            m_GUILayer->Begin();
            for (Layer* layer : m_LayerStack) {
                layer->OnGUIRender();
            }
            m_GUILayer->End();

            m_Window->Update();

            Time newFrameTime;
            m_DeltaTime = (newFrameTime - lastFrameTime).AsSecondsf();
            lastFrameTime = newFrameTime;
        }
    }

    void Engine::Stop() {
        m_Running = false;
    }

    void Engine::PushLayer(Layer* layer) {
        VANTA_PROFILE_FUNCTION();
        m_LayerStack.PushLayer(layer);
    }

    void Engine::PushOverlay(Layer* overlay) {
        VANTA_PROFILE_FUNCTION();
        m_LayerStack.PushOverlay(overlay);
    }

    void Engine::OnEvent(Event& e) {
        VANTA_PROFILE_FUNCTION();
        
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(EVENT_METHOD(Engine::OnWindowClose));
        dispatcher.Dispatch<WindowMoveEvent>(EVENT_METHOD(Engine::OnWindowMove));
        dispatcher.Dispatch<WindowResizeEvent>(EVENT_METHOD(Engine::OnWindowResize));
        dispatcher.Dispatch<WindowMaximizeEvent>(EVENT_METHOD(Engine::OnWindowMaximize));
        dispatcher.Dispatch<WindowMinimizeEvent>(EVENT_METHOD(Engine::OnWindowMinimize));
        dispatcher.Dispatch<WindowRestoreEvent>(EVENT_METHOD(Engine::OnWindowRestore));
        dispatcher.Dispatch<WindowGainFocusEvent>(EVENT_METHOD(Engine::OnWindowGainFocus));
        dispatcher.Dispatch<WindowLoseFocusEvent>(EVENT_METHOD(Engine::OnWindowLoseFocus));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++) {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

    bool Engine::OnWindowClose(WindowCloseEvent&) {
        Stop();
        return true;
    }

    bool Engine::OnWindowMove(WindowMoveEvent&) {
        return false;
    }

    bool Engine::OnWindowResize(WindowResizeEvent& e) {
        VANTA_PROFILE_FUNCTION();
        if (e.Width != 0 && e.Height != 0) {
            m_Minimized = false;
            Renderer::OnWindowResize(e.Width, e.Height);
        }
        return false;
    }

    bool Engine::OnWindowMaximize(WindowMaximizeEvent&) {
        return false;
    }

    bool Engine::OnWindowMinimize(WindowMinimizeEvent&) {
        m_Minimized = true;
        return false;
    }

    bool Engine::OnWindowRestore(WindowRestoreEvent&) {
        m_Minimized = false;
        return false;
    }

    bool Engine::OnWindowGainFocus(WindowGainFocusEvent&) {
        return false;
    }

    bool Engine::OnWindowLoseFocus(WindowLoseFocusEvent&) {
        return false;
    }

    void Engine::SubmitToMainThread(const std::function<void()>& func) {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
        m_MainThreadQueue.push_back(func);
    }

    void Engine::ExectuteMainThreadQueue() {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        for (auto& func : m_MainThreadQueue)
            func();

        m_MainThreadQueue.clear();
    }
}
