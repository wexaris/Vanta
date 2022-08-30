#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/Scene.hpp"

namespace Vanta {

    Entity::Entity(entt::entity handle, Scene* scene)
        : m_Handle(handle), m_Scene(scene)
    {}

    UUID Entity::GetUUID() const {
        return GetComponent<IDComponent>().ID;
    }

    const std::string& Entity::GetName() const {
        return GetComponent<IDComponent>().Name;
    }
}
