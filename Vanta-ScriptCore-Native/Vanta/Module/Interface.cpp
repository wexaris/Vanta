#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta {
    Scripts::EngineFunctions Internal;
}

VANTA_EXPORT void RegisterEngineFunctions(const Vanta::Scripts::EngineFunctions& funcs) {
    Vanta::Internal = funcs;
}

VANTA_EXPORT const char* const* GetClassList(Vanta::usize& count) {
    auto& list = Vanta::Scripts::Registry::GetClassList();
    count = list.size();
    return list.data();
}

VANTA_EXPORT Vanta::Scripts::ClassFunctions* GetClassFunctions(const char* className) {
    return Vanta::Scripts::Registry::GetClassFunctions(className);
}

VANTA_EXPORT const Vanta::Scripts::ClassField* GetClassFieldList(const char* className, Vanta::usize& count) {
    auto list = Vanta::Scripts::Registry::GetClassFieldList(className);
    if (!list) {
        return nullptr;
    }

    count = list->size();
    return list->data();
}

VANTA_EXPORT const char* const* GetComponentList(Vanta::usize& count) {
    auto& list = Vanta::Scripts::Registry::GetComponentList();
    count = list.size();
    return list.data();
}

VANTA_EXPORT Vanta::usize GetComponentHash(const char* componentName) {
    return Vanta::Scripts::Registry::GetComponentHash(componentName);
}

VANTA_EXPORT void Destroy(void* data) {
    delete data;
}
