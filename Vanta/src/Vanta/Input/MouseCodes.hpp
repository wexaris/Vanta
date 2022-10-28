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
}

inline std::ostream& operator<<(std::ostream& os, Vanta::MouseCode key) {
    return os << static_cast<std::underlying_type_t<Vanta::MouseCode>>(key);
}
