#include "vantapch.hpp"
#include "Vanta/Entity/Components.hpp"
#include "Vanta/Entity/Entity.hpp"
#include "Vanta/Entity/Scene.hpp"

namespace Vanta {

    Entity::Entity(entt::entity handle, Scene* scene)
        : m_Handle(handle), m_Scene(scene)
    {}

    const std::string& Entity::GetName() {
        return GetComponent<IDComponent>().Name;
    }
}
