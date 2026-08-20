#pragma once
#include "Vanta/Scripts/Instance.hpp"

namespace Vanta {
    namespace Scripts {

        class NativeScriptInstanceHandle : public ScriptInstanceHandle {
        public:
            NativeScriptInstanceHandle(void* object) : m_RuntimeObject(object) {}
            ~NativeScriptInstanceHandle();

            void* GetRuntimeObject() const override { return m_RuntimeObject;  }
            void Release() override;

        private:
            void* m_RuntimeObject = nullptr;
        };
    }
}
