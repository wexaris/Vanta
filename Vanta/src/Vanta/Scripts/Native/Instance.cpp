#include "vantapch.hpp"
#include "Vanta/Scripts/Native/Instance.hpp"
#include "Vanta/Scripts/Native/ScriptEngine.hpp"

namespace Vanta {
    namespace Scripts {

        NativeScriptInstanceHandle::~NativeScriptInstanceHandle() {
            Release();
        }

        void NativeScriptInstanceHandle::Release() {
            if (m_RuntimeObject) {
                NativeScriptEngine& engine = NativeScriptEngine::Get();
                engine.ReleaseObject(m_RuntimeObject);
                m_RuntimeObject = nullptr;
            }
        }
    }
}
