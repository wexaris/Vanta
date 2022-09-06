#pragma once

namespace Vanta {

    class UniformBuffer {
    public:
        virtual ~UniformBuffer() = default;

        virtual void SetData(const void* data, uint32 size, uint32 offset = 0) = 0;

        static Ref<UniformBuffer> Create(uint32 size, uint32 binding);
    };
}
