#pragma once
#include "../Scene/Components.hpp"

#define VANTA_SCRIPT(name) \
    public: \
        constexpr static const char* GetClassName() { return ReflectionData::GetName(); } \
        static const ::std::vector<::Vanta::Native::ClassField>& GetClassFields() { return ReflectionData::GetFields(); } \
        static ::Vanta::Entity* InvokeConstructor(::Vanta::UUID entityID) { return new name(entityID); } \
        static void InvokeOnCreate(::Vanta::Entity* instance) { ((name*)instance)->OnCreate(); } \
        static void InvokeOnUpdate(::Vanta::Entity* instance, double delta) { ((name*)instance)->OnUpdate(delta); } \
        static void InvokeOnDestroy(::Vanta::Entity* instance) { ((name*)instance)->OnDestroy(); } \
    private: \
        class ReflectionData { \
            constexpr static const char* s_Name = #name; \
            static ::std::vector<::Vanta::Native::ClassField> s_Fields; \
        public: \
            using Self = name; \
            constexpr static const char* GetName() { return s_Name; } \
            static const ::std::vector<::Vanta::Native::ClassField>& GetFields() { return s_Fields; } \
            static bool RegisterField(const char* type, const char* name, void(*getter)(::Vanta::Entity*, void*), void(*setter)(::Vanta::Entity*, const void*)) { \
                s_Fields.push_back(::Vanta::Native::ClassField{ type, name, \
                    (void (*)(::Vanta::Native::ScriptObject*, void*))getter, \
                    (void (*)(::Vanta::Native::ScriptObject*, const void*))setter \
                }); \
                return true; \
            } \
        }; \
        name(::Vanta::UUID entityID) : ::Vanta::Entity(entityID) {} \
        inline static bool CONCAT(_class_, name) = ::Vanta::Native::Registry::RegisterClass(GetClassName(), \
            &name::InvokeConstructor, &name::InvokeOnCreate, &name::InvokeOnUpdate, &name::InvokeOnDestroy);

#define VANTA_FIELD(type, name) \
    static void CONCAT(_Getter_, name)(::Vanta::Entity* instance, void* buffer) { \
        auto inst = (ReflectionData::Self*)instance; \
        memcpy(buffer, (const void*)&inst->name, sizeof(type)); \
    } \
    static void CONCAT(_Setter_, name)(::Vanta::Entity* instance, const void* buffer) { \
        auto inst = (ReflectionData::Self*)instance; \
        memcpy((void*)&inst->name, buffer, sizeof(type)); \
    } \
    inline static bool CONCAT(_field_, name) = ::Vanta::Native::Registry::RegisterClassField(GetClassName(), #name, #type, \
        &CONCAT(ReflectionData::Self::_Getter_, name), &CONCAT(ReflectionData::Self::_Setter_, name));

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
                return T(0);
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
 