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

        static Ref<Scene> Copy(const Ref<Scene>& other);

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

        /// <summary>
        /// Add a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args>
        Component& AddComponent(Entity entity, Args&&... args) {
            VANTA_ASSERT(!HasComponent<Component>(entity), "Entity already has component: {}", typeid(Component).name());
            Component& component = m_Registry.emplace<Component>(entity, std::forward<Args>(args)...);
            OnComponentAdded(entity, component);
            return component;
        }

        /// <summary>
        /// Add to replace a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args>
        Component& AddOrReplaceComponent(Entity entity, Args&&... args) {
            Component& component = m_Registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
            OnComponentAdded(entity, component);
            return component;
        }

        /// <summary>
        /// Remove a given component from an entity.
        /// </summary>
        template<typename Component>
        void RemoveComponent(Entity entity) {
            VANTA_ASSERT(HasComponent<Component>(entity), "Entity does not have component: {}", typeid(Component).name());
            m_Registry.remove<Component>(entity);
        }

        /// <summary>
        /// Get a given component from an entity.
        /// </summary>
        template<typename Component>
        Component& GetComponent(Entity entity) {
            VANTA_ASSERT(HasComponent<Component>(entity), "Entity does not have component: {}", typeid(Component).name());
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
        Camera* GetActiveCamera();

        entt::registry& GetRegistry() { return m_Registry; }

    private:
        using Buffers = Buffer<TransformComponentBuffers>;

        entt::registry m_Registry;
        entt::entity m_ActiveCameraEntity;
        glm::uvec2 m_ViewportSize;

        Buffers m_DataBuffer;
        DispatchBarrier m_UpdateBarrier;

        void OnScriptUpdate(double delta);
        void OnPhysicsUpdate(double delta);
        void OnRender(double delta, Camera* camera);

        template<typename T>
        void OnComponentAdded(entt::entity /*entity*/, T& /*component*/) {}

        template<>
        void OnComponentAdded<CameraComponent>(entt::entity entity, CameraComponent& component) {
            // Set active camera, if one's not set
            if (!GetActiveCamera()) {
                m_ActiveCameraEntity = entity;
            }

            component.Camera.Resize(m_ViewportSize.x, m_ViewportSize.y);
        }
    };
}
