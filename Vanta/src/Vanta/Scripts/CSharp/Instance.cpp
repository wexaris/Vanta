#include "vantapch.hpp"
#include "Vanta/Scripts/CSharp/ScriptEngine.hpp"
#include "Vanta/Scripts/CSharp/Instance.hpp"

namespace Vanta {
    namespace Scripts {

        CSharpScriptInstanceHandle::~CSharpScriptInstanceHandle() {
            Release();
        }

        void* CSharpScriptInstanceHandle::GetRuntimeObject() const {
            if (m_GcHandle == 0) return nullptr;
            CSharpScriptEngine& engine = CSharpScriptEngine::Get();
            return engine.GetObjectByHandle(m_GcHandle);
        }

        void CSharpScriptInstanceHandle::Release() {
            if (m_GcHandle != 0) {
                CSharpScriptEngine& engine = CSharpScriptEngine::Get();
                engine.ReleaseObject(m_GcHandle);
                m_GcHandle = 0;
            }
        }
    }
}
