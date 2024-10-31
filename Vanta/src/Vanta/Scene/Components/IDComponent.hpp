#pragma once

namespace Vanta {

    struct IDComponent {
        UUID ID;
        std::string Name;
        IDComponent(const IDComponent&) = default;
        IDComponent(const std::string& name, UUID uuid) : ID(uuid), Name(name) {}
    };
}