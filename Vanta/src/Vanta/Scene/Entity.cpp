#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/Scene.hpp"

namespace Vanta {

    Entity::Entity(entt::entity handle, Scene* scene)
        : m_Handle(handle), m_Scene(scene)
    {}

    const std::string& Entity::GetName() {
        return GetComponent<IDComponent>().Name;
    }
}
