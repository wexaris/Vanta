#pragma once
#include "Vanta/Entity/Components.hpp"
#include "Vanta/Event/WindowEvent.hpp"
#include "Vanta/Render/Camera.hpp"

#include <entt/entt.hpp>

namespace Vanta {

    class Entity;

    class Scene {
    public:
        Scene();
        ~Scene();

        void OnRuntimeBegin();
        void OnRuntimeEnd();

        void OnSimulationBegin();
        void OnSimulationEnd();

        void OnUpdateRuntime(double delta);
        void OnUpdateSimulation(double delta);
        void OnUpdateEditor(double delta);

        Entity CreateEntity(const std::string& name/*, UUID id*/);
        Entity DuplicateEntity(Entity entity);
        void DestroyEntity(Entity entity);

        template<typename T, typename... Args>
        T& AddComponent(Entity entity, Args&&... args) {
            T& component = m_Registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
            OnComponentAdded(entity, component);
            return component;
        }

        template<typename T>
        void RemoveComponent(Entity entity) {
            return m_Registry.remove<T>(entity);
        }

        template<typename Component>
        Component& GetComponent(Entity entity) {
            if constexpr (Buffers::HasComponent<Component>())
                return m_DataBuffer.Get<Component>(m_Registry, entity);
            else 
                return m_Registry.get<Component>(entity);
        }

        template<typename Component>
        bool HasComponent(Entity entity) {
            return m_Registry.any_of<Component>(entity);
        }

        void OnWindowResize(WindowResizeEvent& e);

        void SetActiveCamera(Entity& camera);
        Entity GetActiveCamera();

        entt::registry& GetRegistry() { return m_Registry; }

    private:
        using Buffers = Buffer<TransformComponentBuffers>;

        entt::registry m_Registry;
        std::optional<entt::entity> m_ActiveCamera = std::nullopt;
        glm::uvec2 m_ViewportSize;

        Buffers m_DataBuffer;

        void OnScriptUpdate(double delta);
        void OnPhysicsUpdate(double delta);
        void OnRender(double delta);

        template<typename T>
        void OnComponentAdded(entt::entity /*entity*/, T& /*component*/) {}

        template<>
        void OnComponentAdded<CameraComponent>(entt::entity entity, CameraComponent& component) {
            // Set active camera, if one's not set
            if (!m_ActiveCamera.has_value())
                m_ActiveCamera = entity;

            component.Camera->Resize(m_ViewportSize.x, m_ViewportSize.y);
        }
    };
}
