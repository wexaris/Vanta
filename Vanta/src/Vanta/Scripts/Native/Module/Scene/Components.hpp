#pragma once
#include "../Math/Math.hpp"

#define EXPOSE_COMPONENT(name) \
    static bool CONCAT(_register_, name);

#define REGISISTER_COMPONENT(name) \
    bool name::CONCAT(_register_, name) = ::Vanta::Native::Registry::RegisterComponent(#name, typeid(name).hash_code());

namespace Vanta {

    struct TransformComponentt {
        EXPOSE_COMPONENT(TransformComponentt);

    public:
        Vector3& GetPosition() {
            static Vector3 instance;
            return instance;
        }
    };

    REGISISTER_COMPONENT(TransformComponentt)
    
}
