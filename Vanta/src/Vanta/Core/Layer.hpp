#pragma once
#include "Vanta/Event/Event.hpp"

namespace Vanta {

    class Layer {
    public:
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnUpdate(double /*delta*/) {}
        virtual void OnGUIRender() {}

        virtual void OnEvent(Event&) {}

        const std::string& GetName() const { return m_Name; }

    protected:
        Layer(const std::string& name = "Layer")
            : m_Name(name) {}

        std::string m_Name;
    };
}
