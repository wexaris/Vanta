#pragma once

namespace Vanta {
    class GraphicsContext {
    public:
        GraphicsContext() = default;
        virtual ~GraphicsContext() = default;

        virtual void SwapBuffers() = 0;

        static Box<GraphicsContext> Create(void* window);
    };
}
