#pragma once
#include "../Scene/Components.hpp"

#define VANTA_SCRIPT(name) \
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
        Entity(UUID id = 0) : m_ID(id) {}
        virtual ~Entity() = default;

        operator bool() const { return m_ID != 0; }

        Entity GetEntityByName(const char* name) {
            return Internal.Entity_GetEntityByName(name);
        }

        template<typename T>
        T GetComponent() {
            if (!HasComponent<T>()) {
                Log::Error("Entity doesn't have the requested component!");
                return 0;
            }
            return T(m_ID);
        }

        template<typename T>
        bool HasComponent() {
            return Internal.Entity_HasComponent(m_ID, typeid(T).hash_code());
        }

    protected:
        virtual void OnCreate() {}
        virtual void OnUpdate(double) {}
        virtual void OnDestroy() {}

        
    private:
        UUID m_ID;
    };
}
 