#include <vanta-test-utils/CoreTestsCommon.hpp>

namespace Testing {

    bool TestEvents_MousePress(MouseButtonPressEvent&) { return true; }
    bool TestEvents_KeyPress(KeyPressEvent&) { return true; }

    bool TestEvents() {
        auto mousePress = MouseButtonPressEvent(Mouse::ButtonLeft);
        EventDispatcher dispatcher(mousePress);

        TRUE_OR_FAIL(dispatcher.Dispatch<MouseButtonPressEvent>(TestEvents_MousePress));
        TRUE_OR_FAIL(!dispatcher.Dispatch<KeyPressEvent>(TestEvents_KeyPress));

        return true;
    }
}
