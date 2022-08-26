#include "vantapch.hpp"
#include "Platform/Windows/Core/Window.hpp"
#include "Vanta/Event/KeyEvent.hpp"
#include "Vanta/Event/MouseEvent.hpp"
#include "Vanta/Event/WindowEvent.hpp"
#include "Vanta/Input/Input.hpp"
#include "Vanta/Render/GraphicsAPI.hpp"

#include <GLFW/glfw3.h>

void GLFWError(int code, const char* msg) {
    VANTA_CORE_ERROR("GLFW Error [{}]: {}", code, msg);
}

namespace Vanta {
    WindowsWindow::WindowsWindow(const WindowParams& params) {
        VANTA_PROFILE_FUNCTION();

        m_Data.Title = params.Title;
        m_Data.Width = params.Width;
        m_Data.Height = params.Height;
        m_Data.VerticalSync = params.VerticalSync;

        { // Initailize GLFW
            VANTA_PROFILE_SCOPE("glfwInit()");
            auto good = glfwInit();
            VANTA_CORE_ASSERT(good, "Failed to initialize GLFW!");
            glfwSetErrorCallback((GLFWerrorfun)GLFWError);
        }

        { // Save video mode
            VANTA_PROFILE_SCOPE("glfwGetVideoMode()");
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            m_VideoMode = glfwGetVideoMode(monitor);
            VANTA_CORE_INFO("Video mode: {}x{}@{}Hz (r{}g{}b{})",
                m_VideoMode->width, m_VideoMode->height, m_VideoMode->refreshRate,
                m_VideoMode->redBits, m_VideoMode->greenBits, m_VideoMode->blueBits);
        }

        { // Create new window
            VANTA_PROFILE_SCOPE("glfwCreateWindow()");

            // Set context parameters
            switch (GraphicsAPI::GetAPI()) {
            case GraphicsAPI::OpenGL:
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef VANTA_DEBUG
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
                break;
            default:
                VANTA_UNREACHABLE("Invalid graphics API!");
            }
            
            m_Window = glfwCreateWindow(params.Width, params.Height, params.Title.c_str(), NULL, NULL);
            VANTA_CORE_ASSERT(m_Window, "Failed to create GLFW window!");
        }

        m_Context = GraphicsContext::Create(m_Window);

        int posX, posY;
        glfwGetWindowPos(m_Window, &posX, &posY);
        m_Data.PosX = posX;
        m_Data.PosY = posY;

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetMode(m_Data.Mode, m_Data.Width, m_Data.Height);
        SetVSync(m_Data.VerticalSync);
        SetIcon(params.IconPath);

        EventCallbackSetup();
    }

    WindowsWindow::~WindowsWindow() {
        VANTA_PROFILE_FUNCTION();
        {
            VANTA_PROFILE_SCOPE("glfwTerminate()");
            glfwTerminate();
        }
    }

    void WindowsWindow::EventCallbackSetup() {
        VANTA_PROFILE_FUNCTION();

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            switch (action) {
            case GLFW_PRESS: {
                KeyPressEvent event(static_cast<KeyCode>(key));
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleaseEvent event(static_cast<KeyCode>(key));
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressEvent event(static_cast<KeyCode>(key));
                data.EventCallback(event);
                break;
            }
            default:
                VANTA_CORE_CRITICAL("Unrecognised key action from GLFW!");
                break;
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint keycode) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            KeyTypeEvent event(static_cast<KeyCode>(keycode));
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int /*mods*/) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressEvent event(static_cast<MouseCode>(button));
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleaseEvent event(static_cast<MouseCode>(button));
                data.EventCallback(event);
                break;
            }
            default:
                VANTA_CORE_CRITICAL("Unrecognised mouse button action from GLFW!");
                break;
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            static bool s_FirstPos = true;
            static double s_PrevX = 0, s_PrevY = 0;

            if (s_FirstPos) {
                s_PrevX = xPos;
                s_PrevY = yPos;
                s_FirstPos = false;
            }

            float offsetX = (float)(xPos - s_PrevX);
            float offsetY = (float)(s_PrevY - yPos);

            s_PrevX = xPos;
            s_PrevY = yPos;

            MouseMoveEvent event((float)xPos, (float)yPos, offsetX, offsetY);
            data.EventCallback(event);
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            MouseScrollEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.PosX = x;
            data.PosY = y;
            if (!data.EventCallback) return;

            WindowMoveEvent event(x, y);
            data.EventCallback(event);
        });

        // Size based on DPI scaling
        /*glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;
            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });*/

        // Size based on pixels
        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;
            if (!data.EventCallback) return;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            if (maximized) {
                WindowMaximizeEvent event;
                data.EventCallback(event);
            }
            else {
                WindowRestoreEvent event;
                data.EventCallback(event);
            }
        });

        glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int minimized) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            if (minimized) {
                WindowMinimizeEvent event;
                data.EventCallback(event);
            }
            else {
                WindowRestoreEvent event;
                data.EventCallback(event);
            }
        });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (!data.EventCallback) return;

            if (focused) {
                WindowGainFocusEvent event;
                data.EventCallback(event);
            }
            else {
                WindowLoseFocusEvent event;
                data.EventCallback(event);
            }
        });
    }

    void WindowsWindow::Update() {
        VANTA_PROFILE_RENDER_FUNCTION();
        m_Context->SwapBuffers();
    }

    void WindowsWindow::SetTitle(const std::string& title) {
        VANTA_PROFILE_FUNCTION();
        m_Data.Title = title;
        glfwSetWindowTitle(m_Window, title.c_str());
    }

    void WindowsWindow::SetSize(uint width, uint height) {
        VANTA_PROFILE_FUNCTION();
        m_Data.Width = width;
        m_Data.Height = height;
        glfwSetWindowSize(m_Window, m_Data.Width, m_Data.Height);
    }

    void WindowsWindow::SetVSync(bool enabled) {
        VANTA_PROFILE_FUNCTION();
        m_Data.VerticalSync = enabled;
        if (m_Data.VerticalSync) { glfwSwapInterval(1); }
        else { glfwSwapInterval(0); }
    }

    void WindowsWindow::SetMode(WindowMode mode, uint width, uint height) {
        VANTA_PROFILE_FUNCTION();

        if (m_Data.Mode == mode) {
            return;
        }

        // If currently windowed, save window position and size
        if (m_Data.Mode == WindowMode::Windowed) {
            m_PrevWindowedParams.Width = m_Data.Width;
            m_PrevWindowedParams.Height = m_Data.Height;
            m_PrevWindowedParams.PosX = m_Data.PosX;
            m_PrevWindowedParams.PosY = m_Data.PosY;
        }

        m_Data.Mode = mode;

        GLFWmonitor* monitor = nullptr;

        switch (mode) {
        case WindowMode::Windowed:
            m_Data.Width = (width != 0 && height != 0) ? width : m_PrevWindowedParams.Width;
            m_Data.Height = (width != 0 && height != 0) ? height : m_PrevWindowedParams.Height;
            break;

        case WindowMode::Borderless:
            m_Data.Width = m_VideoMode->width;
            m_Data.Height = m_VideoMode->height;
            monitor = glfwGetPrimaryMonitor();
            break;

        case WindowMode::FullScreen:
            m_Data.Width = (width != 0 && height != 0) ? width : m_PrevWindowedParams.Width;
            m_Data.Height = (width != 0 && height != 0) ? height : m_PrevWindowedParams.Height;
            monitor = glfwGetPrimaryMonitor();
            break;

        default:
            VANTA_UNREACHABLE("Invalid window mode!");
            break;
        }

        glfwSetWindowMonitor(m_Window, monitor, m_Data.PosX, m_Data.PosY,
            m_Data.Width, m_Data.Height, m_VideoMode->refreshRate);
    }

    void WindowsWindow::SetIcon(const Path&) {
        VANTA_PROFILE_FUNCTION();
        // TODO: Load icon from path
        glfwSetWindowIcon(m_Window, 0, nullptr);
        VANTA_UNIMPLEMENTED();
    }
}
