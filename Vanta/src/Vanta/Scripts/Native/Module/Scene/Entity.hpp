#pragma once

namespace Vanta {
    namespace NativeImpl {

        void RegisterComponent(const char* componentName, usize componentID);

        uint64 Entity_GetEntityByName(const char* name);
        bool Entity_HasComponent(UUID entityID, usize componentID);
    }
}
