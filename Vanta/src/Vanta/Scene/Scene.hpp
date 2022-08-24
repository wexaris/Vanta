#pragma once
#include "Vanta/Event/WindowEvent.hpp"
#include "Vanta/Scene/Components.hpp"
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
        void OnUpdateSimulation(double delta, Camera* camera);
        void OnUpdateEditor(double delta, Camera* camera);

        bool IsValid(Entity entity) const;
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
        bool HasComponent(Entity entity) const {
            return m_Registry.any_of<Component>(entity);
        }

        void OnViewportResize(uint width, uint height);

        void SetActiveCameraEntity(Entity& camera);
        Entity GetActiveCameraEntity();
        Camera* GetActiveCamera() { return m_ActiveCamera.get(); }

        entt::registry& GetRegistry() { return m_Registry; }

    private:
        using Buffers = Buffer<TransformComponentBuffers>;

        entt::registry m_Registry;
        Ref<Camera> m_ActiveCamera = nullptr;
        std::optional<entt::entity> m_ActiveCameraEntity = std::nullopt;
        glm::uvec2 m_ViewportSize;

        Buffers m_DataBuffer;

        void OnScriptUpdate(double delta);
        void OnPhysicsUpdate(double delta);
        void OnRender(double delta, Camera* camera);

        template<typename T>
        void OnComponentAdded(entt::entity /*entity*/, T& /*component*/) {}

        template<>
        void OnComponentAdded<CameraComponent>(entt::entity entity, CameraComponent& component) {
            // Set active camera, if one's not set
            if (!m_ActiveCamera) {
                m_ActiveCamera = component.Camera;
                m_ActiveCameraEntity = entity;
            }

            component.Camera->Resize(m_ViewportSize.x, m_ViewportSize.y);
        }
    };
}