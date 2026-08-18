#pragma once
#include <Vanta/Math/Vector.hpp>

namespace Vanta {
    namespace Scripts {

        typedef void ScriptObject;

        struct EngineFunctions {
            void (*Log_Trace) (const char*);
            void (*Log_Info) (const char*);
            void (*Log_Warn) (const char*);
            void (*Log_Error) (const char*);

            bool (*Input_IsKeyDown) (uint16);
            bool (*Input_IsMouseDown) (uint16);

            uint64 (*Entity_GetEntityByName) (const char*);
            bool (*Entity_HasComponent) (uint64, usize);

            const Vector3& (*TransformComponent_GetPosition) (uint64);
            void (*TransformComponent_SetPosition) (uint64, const Vector3&);

            void (*SpriteComponent_SetColor) (uint64, const Vector4&);

            void (*Rigidbody2DComponent_ApplyLinearImpulseToCenter) (uint64, const Vector2&, bool);
        };

        struct ClassFunctions {
            ScriptObject* (*Constructor) (uint64);
            void (*OnCreate) (ScriptObject*);
            void (*OnUpdate) (ScriptObject*, double);
            void (*OnDestroy) (ScriptObject*);
        };

        struct ClassField {
            const char* Name;
            const char* Type;
            void (*Getter)(ScriptObject*, void*);
            void (*Setter)(ScriptObject*, const void*);
        };
    }

    extern Scripts::EngineFunctions Internal;
}

typedef      void (*RegisterEngineFunctions_Fn) (const Vanta::Scripts::EngineFunctions&);
VANTA_EXPORT void RegisterEngineFunctions       (const Vanta::Scripts::EngineFunctions& funcs);

typedef      const char* const* (*GetClassList_Fn) (Vanta::usize&);
VANTA_EXPORT const char* const* GetClassList       (Vanta::usize& count);

typedef      Vanta::Scripts::ClassFunctions* (*GetClassFunctions_Fn) (const char*);
VANTA_EXPORT Vanta::Scripts::ClassFunctions* GetClassFunctions       (const char* className);

typedef      const Vanta::Scripts::ClassField* (*GetClassFieldList_Fn) (const char*, Vanta::usize&);
VANTA_EXPORT const Vanta::Scripts::ClassField* GetClassFieldList(const char* className, Vanta::usize& count);

typedef      const char* const* (*GetComponentList_Fn) (Vanta::usize&);
VANTA_EXPORT const char* const* GetComponentList       (Vanta::usize& count);

typedef      Vanta::usize (*GetComponentHash_Fn) (const char*);
VANTA_EXPORT Vanta::usize GetComponentHash       (const char* componentName);

typedef      void (*Destroy_Fn) (void*);
VANTA_EXPORT void Destroy       (void*);
