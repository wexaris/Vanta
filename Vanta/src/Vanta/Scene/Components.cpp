#include "vantapch.hpp"
#include "Vanta/Scene/Components.hpp"

#include "Vanta/Render/Renderer2D.hpp"

namespace Vanta {

    void SpriteComponent::Render(double /*delta*/, const glm::mat4& transform) {
        Renderer2D::DrawQuad(transform, { 0.8, 0.3, 0.6, 1.0 });
    }
}
