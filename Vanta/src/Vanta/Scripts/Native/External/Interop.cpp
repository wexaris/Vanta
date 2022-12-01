#pragma once
#include <Vanta/Core/Definitions.hpp>
#include "Interop.hpp"
#include "Registry.hpp"

VANTA_EXPORT const char* const* GetClassList(Vanta::usize& count) {
    auto& list = Vanta::Native::Registry::GetClassList();
    count = list.size();
    return list.data();
}

VANTA_EXPORT Vanta::Native::ClassFunctions* GetClassFunctions(const char* className) {
    return Vanta::Native::Registry::GetClassFunctions(className);
}

VANTA_EXPORT void Destroy(void* data) {
    delete data;
}
