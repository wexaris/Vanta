#pragma once
#include "Vanta/Core/Layer.hpp"
#include "Vanta/Event/WindowEvent.hpp"

struct ImGuiIO;
struct ImGuiStyle;

namespace Vanta {

    class GUILayer : public Layer {
    public:
        GUILayer();
        virtual ~GUILayer() = default;

        void OnAttach() override;
        void OnDetach() override;

        virtual void Begin();
        virtual void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }

        void OnEvent(Event& e) override;

        void SetDarkThemeColors(ImGuiStyle* style = nullptr);

    private:
        bool m_BlockEvents = true;

        bool OnWindowContentScale(WindowContentScaleEvent& e);
        void UpdateGuiStyle();
        void LoadFonts(ImGuiIO* io, float scaleFactor = 1.0f);
    };
}
