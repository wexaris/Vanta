#pragma once
#include "../Scene/Components.hpp"

#define REGISTER_SCRIPT(name) \
    public: \
        static ::Vanta::Entity* InvokeConstructor(::Vanta::UUID entityID) { return new name(entityID); } \
        static void InvokeOnCreate(::Vanta::Entity* instance) { ((name*)instance)->OnCreate(); } \
        static void InvokeOnUpdate(::Vanta::Entity* instance, double delta) { ((name*)instance)->OnUpdate(delta); } \
        static void InvokeOnDestroy(::Vanta::Entity* instance) { ((name*)instance)->OnDestroy(); } \
    private: \
        name(::Vanta::UUID entityID) : ::Vanta::Entity(entityID) {} \
        inline static bool CONCAT(_register_, name) = ::Vanta::Native::Registry::RegisterClass(#name, \
            &name::InvokeConstructor, &name::InvokeOnCreate, &name::InvokeOnUpdate, &name::InvokeOnDestroy);

namespace Vanta {

    class Entity {
    public:
        Entity(UUID id) : m_ID(id) {}
        virtual ~Entity() = default;

    protected:
        virtual void OnCreate() {}
        virtual void OnUpdate(double) {}
        virtual void OnDestroy() {}

        //template<typename T>
        //T& GetComponent() {
        //    return m_ID.GetComponent<T>();
        //}

    private:
        UUID m_ID;
    };
}
 