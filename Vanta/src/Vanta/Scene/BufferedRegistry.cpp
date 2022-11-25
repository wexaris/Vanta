#include "vantapch.hpp"
#include "Vanta/Scene/BufferedRegistry.hpp"

namespace Vanta {

    static usize s_BufferIdx = 0;

    usize GetBufferIndex() {
        return s_BufferIdx;
    }

    void SetBufferIndex(usize idx) {
        s_BufferIdx = idx;
    }
}
