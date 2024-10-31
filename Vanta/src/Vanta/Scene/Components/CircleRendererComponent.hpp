#pragma once

namespace Vanta {

    struct CircleRendererComponent {
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
        float Thickness = 1.f;
        float Fade = 0.01f;
    };
}
