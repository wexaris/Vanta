#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta {
    Native::EngineFunctions Internal;
}

VANTA_EXPORT void RegisterEngineFunctions(const Vanta::Native::EngineFunctions& funcs) {
    Vanta::Internal = funcs;
}

VANTA_EXPORT const char* const* GetClassList(Vanta::usize& count) {
    auto& list = Vanta::Native::Registry::GetClassList();
    count = list.size();
    return list.data();
}

VANTA_EXPORT Vanta::Native::ClassFunctions* GetClassFunctions(const char* className) {
    return Vanta::Native::Registry::GetClassFunctions(className);
}

VANTA_EXPORT const Vanta::Native::ClassField* GetClassFieldList(const char* className, Vanta::usize& count) {
    auto list = Vanta::Native::Registry::GetClassFieldList(className);
    if (!list) {
        return nullptr;
    }

    count = list->size();
    return list->data();
}

VANTA_EXPORT const char* const* GetComponentList(Vanta::usize& count) {
    auto& list = Vanta::Native::Registry::GetComponentList();
    count = list.size();
    return list.data();
}

VANTA_EXPORT Vanta::usize GetComponentHash(const char* componentName) {
    return Vanta::Native::Registry::GetComponentHash(componentName);
}

VANTA_EXPORT void Destroy(void* data) {
    delete data;
}
