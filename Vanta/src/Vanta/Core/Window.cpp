#include "vantapch.hpp"
#include "Vanta/Core/Window.hpp"

#include "Platform/Windows/Core/Window.hpp"

namespace Vanta {
    Box<Window> Window::Create(const WindowParams& params) {
        return NewBox<WindowsWindow>(params);
    }
}
