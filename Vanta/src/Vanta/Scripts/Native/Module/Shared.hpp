#pragma once

namespace Vanta {
    namespace Native {

        typedef void ScriptObject;

        struct EngineFunctions {
            void (*Log_Debug) (const char*);
            void (*Log_Info) (const char*);
            void (*Log_Warn) (const char*);
            void (*Log_Error) (const char*);
        };

        struct ClassFunctions {
            ScriptObject* (*Constructor) (uint64);
            void (*OnCreate) (ScriptObject*);
            void (*OnUpdate) (ScriptObject*, double);
            void (*OnDestroy) (ScriptObject*);
        };
    }
}

typedef      void (*RegisterEngineFunctions_Fn) (const Vanta::Native::EngineFunctions&);
VANTA_EXPORT void RegisterEngineFunctions        (const Vanta::Native::EngineFunctions& funcs);

typedef      const char* const* (*GetClassList_Fn) (Vanta::usize&);
VANTA_EXPORT const char* const* GetClassList       (Vanta::usize& count);

typedef      Vanta::Native::ClassFunctions* (*GetClassFunctions_Fn) (const char*);
VANTA_EXPORT Vanta::Native::ClassFunctions* GetClassFunctions       (const char* className);

typedef      const char* const* (*GetComponentList_Fn) (Vanta::usize&);
VANTA_EXPORT const char* const* GetComponentList(Vanta::usize& count);

typedef      Vanta::usize (*GetComponentHash_Fn) (const char*);
VANTA_EXPORT Vanta::usize GetComponentHash(const char* componentName);

typedef      void (*Destroy_Fn) (void*);
VANTA_EXPORT void Destroy       (void*);
