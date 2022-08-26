#pragma once
#include "Vanta/Event/Event.hpp"
#include "Vanta/Input/Input.hpp"

namespace Vanta {
    class MouseButtonEvent : public Event {
    public:
        const MouseCode Button;

        VANTA_GEN_EVENT_CATEGORY(Category::Input | Category::Mouse | Category::MouseButton);

    protected:
        MouseButtonEvent(MouseCode button) : Button(button) {}
    };

    class MouseButtonPressEvent : public MouseButtonEvent {
    public:
        MouseButtonPressEvent(MouseCode button) : MouseButtonEvent(button) {}

        std::string ToString() const override {
            return FMT("MouseButtonPress: {}", Button);
        }

        VANTA_GEN_EVENT_TYPE(MouseButtonPress);
    };

    class MouseButtonReleaseEvent : public MouseButtonEvent {
    public:
        MouseButtonReleaseEvent(MouseCode button) : MouseButtonEvent(button) {}

        std::string ToString() const override {
            return FMT("MouseButtonRelease: {}", Button);
        }

        VANTA_GEN_EVENT_TYPE(MouseButtonRelease);
    };

    class MouseMoveEvent : public Event {
    public:
        const float PosX, PosY;
        const float OffsetX, OffsetY;

        MouseMoveEvent(float posX, float posY, float offsetX, float offsetY) :
            PosX(posX), PosY(posY), OffsetX(offsetX), OffsetY(offsetY) {}

        std::string ToString() const override {
            return FMT("MouseMove: {}, {}", PosX, PosY);
        }

        VANTA_GEN_EVENT_TYPE(MouseMove);
        VANTA_GEN_EVENT_CATEGORY(Category::Input | Category::Mouse);
    };

    class MouseScrollEvent : public Event {
    public:
        const float OffsetX, OffsetY;

        MouseScrollEvent(float offsetX, float offsetY) :
            OffsetX(offsetX), OffsetY(offsetY) {}

        std::string ToString() const override {
            return FMT("MouseScroll: {}, {}", OffsetX, OffsetY);
        }

        VANTA_GEN_EVENT_TYPE(MouseScroll);
        VANTA_GEN_EVENT_CATEGORY(Category::Input | Category::Mouse);
    };
}
