#pragma once
#include <Vanta/Core/Definitions.hpp>
#include <Vanta/Core/Types.hpp>

namespace Vanta {
    
    class EntityScript;

    namespace Native {

        struct ClassFunctions {
            EntityScript* (*Constructor) (uint64);
            void (*OnCreate) (EntityScript*);
            void (*OnUpdate) (EntityScript*, double);
            void (*OnDestroy) (EntityScript*);
        };
    }
}

typedef      const char* const* (*GetClassList_Fn) (Vanta::usize&);
VANTA_EXPORT const char* const* GetClassList       (Vanta::usize& count);

typedef      Vanta::Native::ClassFunctions* (*GetClassFunctions_Fn) (const char*);
VANTA_EXPORT Vanta::Native::ClassFunctions* GetClassFunctions       (const char* className);

typedef      void (*Destroy_Fn) (void*);
VANTA_EXPORT void Destroy       (void*);
