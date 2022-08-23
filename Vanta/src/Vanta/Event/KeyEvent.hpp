#pragma once
#include "Vanta/Event/Event.hpp"
#include "Vanta/Input/Input.hpp"

namespace Vanta {
    class KeyEvent : public Event {
    public:
        const KeyCode Key;

        VANTA_GEN_EVENT_CATEGORY(Category::Input | Category::Keyboard);

    protected:
        KeyEvent(KeyCode key) : Key(key) {}
    };

    class KeyPressEvent : public KeyEvent {
    public:
        const uint RepeatCount;

        KeyPressEvent(KeyCode key) : KeyEvent(key),
            RepeatCount(Input::IncrementKeyRepeatCount(key))
        {}

        bool IsRepeat() const { return RepeatCount > 0; }

        std::string ToString() const override {
            return FMT("KeyPress: {} ({} repeats)",
                Key,
                RepeatCount);
        }

        VANTA_GEN_EVENT_TYPE(KeyPress);
    };

    class KeyTypeEvent : public KeyEvent {
    public:
        KeyTypeEvent(KeyCode key) : KeyEvent(key) {}

        std::string ToString() const override {
            return FMT("KeyType: {}", Key);
        }

        VANTA_GEN_EVENT_TYPE(KeyType);
    };

    class KeyReleaseEvent : public KeyEvent {
    public:
        KeyReleaseEvent(KeyCode key) : KeyEvent(key) {
            Input::ResetKeyRepeatCount(key);
        }

        std::string ToString() const override {
            return FMT("KeyRelease: {}", Key);
        }

        VANTA_GEN_EVENT_TYPE(KeyRelease);
    };
}
