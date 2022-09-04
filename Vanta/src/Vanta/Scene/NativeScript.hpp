#pragma once
#include "Vanta/Scene/Entity.hpp"

namespace Vanta {

    class NativeScript {
    public:
        virtual ~NativeScript() = default;

    protected:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(double delta) { UNUSED(delta); }

        template<typename T>
        T& GetComponent() {
            return m_Entity.GetComponent<T>();
        }

    private:
        friend class Scene;

        Entity m_Entity;
    };
}
