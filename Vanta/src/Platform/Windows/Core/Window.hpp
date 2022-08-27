#pragma once
#include "Vanta/Core/Window.hpp"
#include "Vanta/Render/GraphicsContext.hpp"

#include <GLFW/glfw3.h>

namespace Vanta {
    struct WindowedParams {
        uint Width, Height;
        uint PosX, PosY;
    };
    
    struct WindowData {
        std::string Title;
        WindowMode Mode;
        uint Width, Height;
        uint PosX, PosY;
        bool VerticalSync;
        CursorMode Cursor;
        Window::EventCallbackFn EventCallback;
    };

    class WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowParams& params);
        ~WindowsWindow();

        void Update() override;

        void SetTitle(const std::string& title) override;
        void SetSize(uint width, uint height) override;
        void SetVSync(bool enabled) override;
        void SetMode(WindowMode mode, uint width, uint height) override;
        void SetIcon(const Path& path) override;
        void SetCursorMode(CursorMode mode) override;
        void SetEventCallback(EventCallbackFn fn) override { m_Data.EventCallback = fn; }

        std::string GetTitle() const override { return m_Data.Title; }
        uint GetWidth() const override        { return m_Data.Width; }
        uint GetHeight() const override       { return m_Data.Height; }
        bool GetVSync() const override        { return m_Data.VerticalSync; }
        WindowMode GetMode() const override   { return m_Data.Mode; }
        CursorMode GetCursorMode() const override { return m_Data.Cursor; }

        void* GetNativeWindow() const override { return m_Window; }

    private:
        GLFWwindow* m_Window;
        const GLFWvidmode* m_VideoMode;
        WindowData m_Data;
        WindowedParams m_PrevWindowedParams;
        Box<GraphicsContext> m_Context;

        void EventCallbackSetup();
    };
}