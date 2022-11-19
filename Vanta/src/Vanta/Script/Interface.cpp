#include "vantapch.hpp"
#include "Vanta/Script/Interface.hpp"

#include <mono/metadata/object.h>

namespace Vanta {

#define VANTA_ADD_INTERNAL_CALL(name) mono_add_internal_call("Vanta.Internal::" #name, name)

    static void Info(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        VANTA_INFO(str);
        mono_free(str);
    }

    void Interface::RegisterFunctions() {
        VANTA_ADD_INTERNAL_CALL(Info);
    }
}
