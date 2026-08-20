#pragma once
#include "Vanta/Scripts/Instance.hpp"

namespace Vanta {
    namespace Scripts {

        class CSharpScriptInstanceHandle : public ScriptInstanceHandle {
        public:
            CSharpScriptInstanceHandle(uint32 gcHandle) : m_GcHandle(gcHandle) {}
            ~CSharpScriptInstanceHandle();

            void* GetRuntimeObject() const override;
            void Release() override;

        private:
            uint32 m_GcHandle = 0;
        };
    }
}
