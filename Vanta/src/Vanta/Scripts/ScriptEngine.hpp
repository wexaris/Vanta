#pragma once
#include "Vanta/Scene/Entity.hpp"

namespace Vanta {

    class Scene;

    namespace Scripts {

        class ScriptClass;
        class ScriptInstance;
        class ScriptFieldInstance;
        
        class ScriptEngine {
        public:
            virtual ~ScriptEngine() = default;

            virtual void Init() = 0;
            virtual void Shutdown() = 0;

            virtual void ReloadAssembly() = 0;

            virtual void RuntimeBegin(Scene* context) = 0;
            virtual void RuntimeEnd() = 0;

            // Classes
            virtual bool EntityClassExists(const std::string& className) const = 0;
            virtual Ref<ScriptClass> GetEntityClass(const std::string& className) const = 0;

            virtual Ref<ScriptInstance> Instantiate(const std::string& className, Entity entity) const;

            // Fields
            virtual Opt<ValueRef<const std::unordered_map<std::string, Box<ScriptFieldInstance>>>> GetFieldInstances(Entity entity) const = 0;
            virtual void SetFieldInstance(Entity entity, Box<ScriptFieldInstance> instance) = 0;
            virtual void ClearFieldInstances() = 0;
        };
    }
}
