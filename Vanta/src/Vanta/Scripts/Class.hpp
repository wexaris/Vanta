#pragma once
#include "Vanta/Scene/Entity.hpp"

namespace Vanta {
    namespace Scripts {

        class ScriptInstance;
        class ScriptField;

        class ScriptClass {
        public:
            explicit ScriptClass() = default;
            explicit ScriptClass(std::vector<Ref<ScriptField>> fields);

            virtual void InvokeOnCreate(const ScriptInstance* instance) const = 0;
            virtual void InvokeOnUpdate(const ScriptInstance* instance, double delta) const = 0;
            virtual void InvokeOnDestroy(const ScriptInstance* instance) const = 0;

            const std::unordered_map<std::string, Ref<ScriptField>>& GetFields() const { return m_Fields; }

        protected:
            friend class ScriptInstance;

            std::unordered_map<std::string, Ref<ScriptField>> m_Fields;

            virtual void* InstantiateRuntimeInstance(Entity entity) const = 0;
        };
    }
}
