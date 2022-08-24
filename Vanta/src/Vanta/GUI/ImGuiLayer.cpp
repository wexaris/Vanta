#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/GUI/ImGuiLayer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <ImGuizmo.h>

namespace Vanta {
    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer")
    {}

    void ImGuiLayer::OnAttach() {
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
        
        // Setup fonts
        std::string opensans_bold_path = "Fonts/OpenSans/OpenSans-Bold.ttf";
        std::string opensans_regular_path = "Fonts/OpenSans/OpenSans-Regular.ttf";

        float fontSize = 18.0f;// *2.0f;
        io.Fonts->AddFontFromFileTTF(opensans_bold_path.c_str(), fontSize);
        io.FontDefault = io.Fonts->AddFontFromFileTTF(opensans_regular_path.c_str(), fontSize);

        // Setup style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Since viewports are enabled, tweak WindowRounding/WindowBg so platform windows look the same
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        {
            VANTA_PROFILE_SCOPE("ImGui_OpenGL3_Init()");
            GLFWwindow* window = static_cast<GLFWwindow*>(Engine::Get().GetWindow().GetNativeWindow());
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 430");
        }
    }

    void ImGuiLayer::OnDetach() {
        VANTA_PROFILE_FUNCTION();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::Begin() {
        VANTA_PROFILE_FUNCTION();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void ImGuiLayer::End() {
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

    void ImGuiLayer::OnEvent(Event& e) {
        VANTA_PROFILE_FUNCTION();
        if (m_BlockEvents) {
            ImGuiIO& io = ImGui::GetIO();
            e.Handled |= e.IsCategory(Event::Category::Mouse) & io.WantCaptureMouse;
            e.Handled |= e.IsCategory(Event::Category::Keyboard) & io.WantCaptureKeyboard;
        }
    }
}
