#pragma once
#include "Vanta/Core/Layer.hpp"

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

    private:
        bool m_BlockEvents = true;
    };
}
