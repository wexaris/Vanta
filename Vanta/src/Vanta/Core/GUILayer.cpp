#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Core/GUILayer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <ImGuizmo.h>

namespace Vanta {
    GUILayer::GUILayer()
        : Layer("ImGuiLayer")
    {}

    void GUILayer::OnAttach() {
        VANTA_PROFILE_FUNCTION();

        {
            VANTA_PROFILE_SCOPE("ImGui::CreateContext()");
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
        }

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
        
        UpdateGuiStyle();

        {
            VANTA_PROFILE_SCOPE("ImGui_OpenGL3_Init()");
            GLFWwindow* window = static_cast<GLFWwindow*>(Engine::Get().GetWindow().GetNativeWindow());
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 430");
        }
    }

    void GUILayer::OnDetach() {
        VANTA_PROFILE_FUNCTION();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GUILayer::Begin() {
        VANTA_PROFILE_FUNCTION();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void GUILayer::End() {
        VANTA_PROFILE_FUNCTION();

        ImGuiIO& io = ImGui::GetIO();
        Engine& engine = Engine::Get();
        io.DisplaySize = ImVec2((float)engine.GetWindow().GetWidth(), (float)engine.GetWindow().GetHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void GUILayer::OnEvent(Event& e) {
        VANTA_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowContentScaleEvent>(EVENT_METHOD(GUILayer::OnWindowContentScale));

        if (m_BlockEvents) {
            ImGuiIO& io = ImGui::GetIO();
            e.Handled |= e.IsCategory(Event::Category::Mouse);// & io.WantCaptureMouse;
            e.Handled |= e.IsCategory(Event::Category::Keyboard) & io.WantCaptureKeyboard;
        }
    }

    bool GUILayer::OnWindowContentScale(WindowContentScaleEvent&) {
        VANTA_PROFILE_FUNCTION();
        UpdateGuiStyle();
        return false;
    }

    void GUILayer::UpdateGuiStyle() {
        VANTA_PROFILE_FUNCTION();


        // Get the current content scale factor for the window
        GLFWwindow* window = static_cast<GLFWwindow*>(Engine::Get().GetWindow().GetNativeWindow());
        float scaleFactor = ImGui_ImplGlfw_GetContentScaleForWindow(window);
        
        // Load fonts with the new scale factor
        ImGuiIO& io = ImGui::GetIO();
        LoadFonts(&io, scaleFactor);

        // Reset style to default values
        ImGuiStyle& style = ImGui::GetStyle();
        style = ImGuiStyle();              

        // Set theme colors and other style properties
        SetDarkThemeColors(&style);

        // If viewports are enabled, tweak WindowRounding/WindowBg so platform windows look the same
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        
        // Multiply sizes by the scale factor
        style.ScaleAllSizes(scaleFactor);  
    }

    void  GUILayer::LoadFonts(ImGuiIO* io, float scaleFactor) {
        VANTA_PROFILE_FUNCTION();

        if (!io)
            io = &ImGui::GetIO();

        // Clear the existing fonts to avoid memory leaks and ensure that the new font size is applied correctly
        io->Fonts->Clear();

        // Load fonts with the new scale factor. You can adjust the font size based on the scale factor.
        auto resourceDir = Engine::RuntimeResourceDirectory();
        auto opensansRegularPath = resourceDir / "Fonts/OpenSans/OpenSans-Regular.ttf";
        auto opensansBoldPath = resourceDir / "Fonts/OpenSans/OpenSans-Bold.ttf";

        constexpr float BaseFontSize = 16.0f;
        io->Fonts->AddFontFromFileTTF(opensansRegularPath.string().c_str(), BaseFontSize * scaleFactor);
        io->Fonts->AddFontFromFileTTF(opensansBoldPath.string().c_str(), BaseFontSize * scaleFactor);
    }

    void GUILayer::SetDarkThemeColors(ImGuiStyle* style) {
        VANTA_PROFILE_FUNCTION();

        if (!style)
            style = &ImGui::GetStyle();

        // Set the base dark theme colors
        ImGui::StyleColorsDark(style);
        
        auto& colors = style->Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // Header
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Button
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frame
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tab
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }
}
