#include "vantapch.hpp"
#include "LayerStack.hpp"

namespace Vanta {
    LayerStack::~LayerStack() {
        for (usize i = 0; i < m_Layers.size(); i++) {
            m_Layers[i]->OnDetach();
            delete m_Layers[i];
        }
    }

    void LayerStack::PushLayer(Layer* layer) {
        m_Layers.emplace(m_Layers.begin() + m_InsertIndex, layer);
        m_InsertIndex++;
        layer->OnAttach();
    }

    void LayerStack::PushOverlay(Layer* overlay) {
        m_Layers.emplace_back(overlay);
        overlay->OnAttach();
    }

    void LayerStack::PopLayer(Layer* layer) {
        auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_InsertIndex, layer);
        if (it != m_Layers.begin() + m_InsertIndex) {
            layer->OnDetach();
            m_Layers.erase(it);
            m_InsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay) {
        auto it = std::find(m_Layers.begin() + m_InsertIndex, m_Layers.end(), overlay);
        if (it != m_Layers.end()) {
            overlay->OnDetach();
            m_Layers.erase(it);
        }
    }
}
