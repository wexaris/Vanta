#pragma once

namespace Vanta {
    enum class MouseCode : uint16 {
        Button0 = 0,
        Button1 = 1,
        Button2 = 2,
        Button3 = 3,
        Button4 = 4,
        Button5 = 5,
        Button6 = 6,
        Button7 = 7,
        ButtonLast = Button7,

        ButtonLeft = Button0,
        ButtonRight = Button1,
        ButtonMiddle = Button2
    };
    using Mouse = MouseCode;

    //static std::ostream& operator<<(std::ostream& os, MouseCode key) {
    //    return os << static_cast<std::underlying_type_t<MouseCode>>(key);
    //}
}

template<>
struct fmt::formatter<Vanta::MouseCode> : formatter<std::underlying_type_t<Vanta::MouseCode>> {
    template <typename FormatContext>
    auto format(Vanta::MouseCode key, FormatContext& ctx) const {
        return formatter<std::underlying_type_t<Vanta::MouseCode>>::format(static_cast<std::underlying_type_t<Vanta::MouseCode>>(key), ctx);
    }
};
