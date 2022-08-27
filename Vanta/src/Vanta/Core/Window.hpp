#pragma once
#include "Vanta/Event/Event.hpp"

namespace Vanta {
    enum class WindowMode {
        Windowed,
        FullScreen,
        Borderless
    };

    struct WindowParams {
        std::string Title = "Vanta";
        WindowMode Mode = WindowMode::Windowed;
        uint Width = 1200;
        uint Height = 800;
        bool VerticalSync = true;
        Path IconPath;
    };

    enum class CursorMode {
        Normal,
        Disabled,
        Hidden,
    };

    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void Update() = 0;

        virtual void SetTitle(const std::string& title) = 0;
        virtual void SetSize(uint width, uint height) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual void SetMode(WindowMode mode, uint width = 0, uint height = 0) = 0;
        virtual void SetIcon(const Path& path) = 0;
        virtual void SetCursorMode(CursorMode mode) = 0;
        virtual void SetEventCallback(EventCallbackFn fn) = 0;

        virtual std::string GetTitle() const = 0;
        virtual uint GetWidth() const = 0;
        virtual uint GetHeight() const = 0;
        virtual bool GetVSync() const = 0;
        virtual WindowMode GetMode() const = 0;
        virtual CursorMode GetCursorMode() const = 0;

        virtual void* GetNativeWindow() const = 0;

        static Box<Window> Create(const WindowParams& params);
    };
}
