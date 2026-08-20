#pragma once
#include  "Vanta/Scene/Entity.hpp"

namespace Vanta {
    namespace Scripts {

        class ScriptClass;

        class ScriptInstanceHandle {
        public:
            ScriptInstanceHandle() = default;
            virtual ~ScriptInstanceHandle() = default;

            ScriptInstanceHandle(const ScriptInstanceHandle&) = delete;
            ScriptInstanceHandle& operator=(const ScriptInstanceHandle&) = delete;

            virtual void* GetRuntimeObject() const = 0;
            virtual void Release() = 0;
        };

        class ScriptInstance {
        public:
            ScriptInstance(Ref<ScriptClass> klass, Entity entity);

            virtual void OnCreate() const;
            virtual void OnUpdate(float delta) const;
            virtual void OnDestroy() const;

            Ref<ScriptClass>& GetClass() { return m_ScriptClass; }

            template<typename T>
            T GetFieldValue(const std::string& name) {
                static char buffer[sizeof(T)];
                bool ok = ReadFieldValue(name, buffer);
                if (!ok)
                    return T();
                return *(T*)buffer;
            }

            template<typename T>
            bool SetFieldValue(const std::string& name, const T& value) {
                return WriteFieldValue(name, &value);
            }

            bool ReadFieldValue(const std::string& name, void* buffer);
            bool WriteFieldValue(const std::string& name, const void* data);

            void* GetRuntimeObject() const { return m_InstanceHandle->GetRuntimeObject(); }

        protected:
            Ref<ScriptClass> m_ScriptClass;
            Box<ScriptInstanceHandle> m_InstanceHandle;
        };
    }
}
