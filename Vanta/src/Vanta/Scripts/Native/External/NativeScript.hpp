#pragma once
#include "Interop.cpp"

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define REGISTER_SCRIPT(name) \
    public: \
        static ::Vanta::EntityScript* InvokeConstructor(::Vanta::UUID entityID) { return new name(entityID); } \
        static void InvokeOnCreate(::Vanta::EntityScript* instance) { ((name*)instance)->OnCreate(); } \
        static void InvokeOnUpdate(::Vanta::EntityScript* instance, double delta) { ((name*)instance)->OnUpdate(delta); } \
        static void InvokeOnDestroy(::Vanta::EntityScript* instance) { ((name*)instance)->OnDestroy(); } \
    private: \
        name(::Vanta::UUID entityID) : ::Vanta::EntityScript(entityID) {} \
        inline static bool CONCAT(_register_, name) = ::Vanta::Native::Registry::RegisterClass(#name, \
            &name::InvokeConstructor, &name::InvokeOnCreate, &name::InvokeOnUpdate, &name::InvokeOnDestroy);

namespace Vanta {

    class EntityScript {
    public:
        EntityScript(UUID entityID) : m_EntityID(entityID) {}
        virtual ~EntityScript() = default;

    protected:
        virtual void OnCreate() {}
        virtual void OnUpdate(double) {}
        virtual void OnDestroy() {}

        //template<typename T>
        //T& GetComponent() {
        //    return m_Entity.GetComponent<T>();
        //}

    private:
        UUID m_EntityID;
    };
}
 