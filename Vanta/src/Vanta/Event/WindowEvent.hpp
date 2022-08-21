#pragma once
#include "Vanta/Event/Event.hpp"

namespace Vanta {
    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() = default;

        VANTA_GEN_EVENT_TYPE(WindowClose);
        VANTA_GEN_EVENT_CATEGORY(Category::Window);
    };

    class WindowMoveEvent : public Event {
    public:
        const uint PosX, PosY;

        WindowMoveEvent(uint posX, uint posY) :
            PosX(posX), PosY(posY)
        {}

        std::string ToString() const override {
            return FMT("WindowMove: {}, {}", PosX, PosY);
        }

        VANTA_GEN_EVENT_TYPE(WindowMove);
        VANTA_GEN_EVENT_CATEGORY(Category::Window);
    };

    class WindowResizeEvent : public Event {
    public:
        const uint Width, Height;

        WindowResizeEvent(uint width, uint height) :
            Width(width), Height(height)
        {}

        std::string ToString() const override {
            return FMT("WindowResize: {}, {}", Width, Height);
        }

        VANTA_GEN_EVENT_TYPE(WindowResize);
        VANTA_GEN_EVENT_CATEGORY(Category::Window);
    };

    class WindowMaximizeEvent : public Event {
    public:
        WindowMaximizeEvent() = default;

        VANTA_GEN_EVENT_TYPE(WindowMaximize);
        VANTA_GEN_EVENT_CATEGORY(Category::Window);
    };

    class WindowMinimizeEvent : public Event {
    public:
        WindowMinimizeEvent() = default;

        VANTA_GEN_EVENT_TYPE(WindowMinimize);
        VANTA_GEN_EVENT_CATEGORY(Category::Window);
    };

    class WindowRestoreEvent : public Event {
    public:
        WindowRestoreEvent() = default;

        VANTA_GEN_EVENT_TYPE(WindowRestore);
        VANTA_GEN_EVENT_CATEGORY(Category::Window);
    };

    class WindowGainFocusEvent : public Event {
    public:
        WindowGainFocusEvent() = default;

        VANTA_GEN_EVENT_TYPE(WindowGainFocus);
        VANTA_GEN_EVENT_CATEGORY(Category::Window);
    };

    class WindowLoseFocusEvent : public Event {
    public:
        WindowLoseFocusEvent() = default;

        VANTA_GEN_EVENT_TYPE(WindowLoseFocus);
        VANTA_GEN_EVENT_CATEGORY(Category::Window);
    };
}
