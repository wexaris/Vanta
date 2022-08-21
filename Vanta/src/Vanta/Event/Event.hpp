#pragma once

namespace Vanta {
    class Event {
    public:
        enum class Type {
            None = 0,
            KeyPress, KeyType, KeyRelease,
            MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll,
            WindowClose, WindowMove, WindowResize, WindowMaximize, WindowMinimize, WindowRestore,
            WindowGainFocus, WindowLoseFocus,
        };
        enum Category {
            None = 0,
            Input       = BIT(1),
            Keyboard    = BIT(2),
            Mouse       = BIT(3),
            MouseButton = BIT(4),
            Window      = BIT(5)
        };

        Event() = default;
        virtual ~Event() = default;

        bool Handled = false;

        bool IsCategory(Category category) const { return GetCategoryFlags() & category; }

        virtual std::string ToString() const { return GetName(); }

        virtual constexpr Type GetType() const = 0;
        virtual constexpr int GetCategoryFlags() const = 0;
        virtual constexpr const char* GetName() const = 0;
    };

    class EventDispatcher {
    public:
        EventDispatcher(Event& event) : m_Event(event) {}

        template<typename T, std::enable_if_t<std::is_base_of_v<Event, T>, int> = 0>
        bool Dispatch(std::function<bool(T&)> fn) {
            if (!m_Event.Handled && m_Event.GetType() == T::StaticType) {
                m_Event.Handled = fn(static_cast<T&>(m_Event));
                return m_Event.Handled;
            }
            return false;
        }

    private:
        Event& m_Event;
    };
}

template<typename E>
struct fmt::formatter<E, std::enable_if_t<std::is_base_of_v<Vanta::Event, E>, char>> : formatter<std::string> {
    template <typename FormatContext>
    auto format(const E& e, FormatContext& ctx) const {
        return formatter<std::string>::format(e.ToString(), ctx);
    }
};

#define VANTA_GEN_EVENT_TYPE(name) \
    static constexpr Event::Type StaticType = Event::Type::name;\
    Event::Type GetType() const override { return StaticType; }\
    const char* GetName() const override { return #name; }

#define VANTA_GEN_EVENT_CATEGORY(category) \
    int GetCategoryFlags() const override { return category; }

#define EVENT_METHOD(fn) (std::bind(&fn, this, std::placeholders::_1))
