#pragma once
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scripts/Field.hpp"

namespace Vanta {

    class ScriptInstance;

    class ScriptClass {
    public:
        explicit ScriptClass() = default;
        explicit ScriptClass(std::vector<Ref<ScriptField>> fields);

        virtual void InvokeOnCreate(const ScriptInstance* instance) const = 0;
        virtual void InvokeOnUpdate(const ScriptInstance* instance, double delta) const = 0;
        virtual void InvokeOnDestroy(const ScriptInstance* instance) const = 0;

        const std::unordered_map<std::string_view, Ref<ScriptField>>& GetFields() const { return m_Fields; }

    protected:
        friend class ScriptInstance;

        std::unordered_map<std::string_view, Ref<ScriptField>> m_Fields;

        virtual void* InstantiateRuntimeInstance(Entity entity) const = 0;
    };

    class ScriptInstance {
    public:
        ScriptInstance(Ref<ScriptClass> klass, Entity entity);

        virtual void OnCreate() const;
        virtual void OnUpdate(float delta) const;
        virtual void OnDestroy() const;

        Ref<ScriptClass>& GetClass() { return m_ScriptClass; }

        template<typename T>
        T GetFieldValue(std::string_view name) {
            static char buffer[sizeof(T)];
            bool ok = ReadFieldValue(name, buffer);
            if (!ok)
                return T();
            return *(T*)buffer;
        }

        template<typename T>
        bool SetFieldValue(std::string_view name, const T& value) {
            return WriteFieldValue(name, &value);
        }

        bool ReadFieldValue(std::string_view name, void* buffer);
        bool WriteFieldValue(std::string_view name, const void* data);

        const void* GetRuntimeInstance() const { return m_RuntimeInstance; }

    protected:
        Ref<ScriptClass> m_ScriptClass;
        void* m_RuntimeInstance = nullptr;

    };
}
