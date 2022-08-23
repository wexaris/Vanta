#pragma once
#include "Vanta/Core/Layer.hpp"

namespace Vanta {

    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer();
        virtual ~ImGuiLayer() = default;

        void OnAttach() override;
        void OnDetach() override;

        virtual void Begin();
        virtual void End();

        void OnEvent(Event& e) override;

        //void BlockEvents(bool block) { m_BlockEvents = block; }

    private:
        //bool m_BlockEvents = true;
    };
}
