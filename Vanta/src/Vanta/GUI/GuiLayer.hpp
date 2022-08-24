#pragma once
#include "Vanta/Core/Layer.hpp"

namespace Vanta {

    class GuiLayer : public Layer {
    public:
        GuiLayer();
        virtual ~GuiLayer() = default;

        void OnAttach() override;
        void OnDetach() override;

        virtual void Begin();
        virtual void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }

        void OnEvent(Event& e) override;

    private:
        bool m_BlockEvents = true;
    };
}
