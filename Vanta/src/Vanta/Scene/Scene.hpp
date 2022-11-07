#pragma once
#include "Vanta/Event/WindowEvent.hpp"
#include "Vanta/Scene/BufferedRegistry.hpp"
#include "Vanta/Scene/Dispatch.hpp"
#include "Vanta/Render/Camera.hpp"

class b2World;

namespace Vanta {

    class Entity;

    struct TransformComponent_1 : TransformComponent {};
    struct TransformComponent_2 : TransformComponent {};

    class Scene {
    public:
        using BufferedTransform = BufferedComponent<TransformComponent, TransformComponent_1, TransformComponent_2>;
        using Registry = BufferedRegistry<BufferedTransform>;

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

        bool IsValid(entt::entity entity) const;
        Entity CreateEntity(const std::string& name, UUID uuid = UUID());
        Entity DuplicateEntity(entt::entity entity);
        void DestroyEntity(entt::entity entity);

        /// <summary>
        /// Add a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args>
        Component& AddComponent(entt::entity entity, Args&&... args) {
            VANTA_ASSERT(!HasComponent<Component>(entity), "Entity already has component: {}", typeid(Component).name());
            Component& component = m_Registry.AddComponent<Component>(entity, std::forward<Args>(args)...);
            OnComponentAdded(entity, component);
            return component;
        }

        /// <summary>
        /// Add to replace a given component to an entity.
        /// </summary>
        template<typename Component, typename... Args>
        Component& AddOrReplaceComponent(entt::entity entity, Args&&... args) {
            return m_Registry.AddOrReplaceComponent<Component>(entity, std::forward<Args>(args)...);
        }

        /// <summary>
        /// Remove a given component from an entity.
        /// </summary>
        template<typename Component>
        void RemoveComponent(entt::entity entity) {
            VANTA_ASSERT(HasComponent<Component>(entity), "Entity does not have component: {}", typeid(Component).name());
            m_Registry.RemoveComponent<Component>(entity);
        }

        /// <summary>
        /// Get a given component from an entity.
        /// </summary>
        template<typename Component>
        Component& GetComponent(entt::entity entity) {
            VANTA_ASSERT(HasComponent<Component>(entity), "Entity does not have component: {}", typeid(Component).name());
            return m_Registry.GetComponent<Component>(entity);
        }

        template<typename Component>
        const Component& GetComponent(entt::entity entity) const {
            VANTA_ASSERT(HasComponent<Component>(entity), "Entity does not have component: {}", typeid(Component).name());
            return m_Registry.GetComponent<Component>(entity);
        }

        template<typename Component>
        bool HasComponent(entt::entity entity) const {
            return m_Registry.HasComponent<Component>(entity);
        }

        void OnViewportResize(uint width, uint height);

        void SetActiveCameraEntity(entt::entity camera);
        Entity GetActiveCameraEntity();
        Camera* GetActiveCamera();

        Registry& GetRegistry() { return m_Registry; }

    private:
        Registry m_Registry;
        b2World* m_PhysicsWorld = nullptr;
        ParallelBarrier m_Barrier;

        glm::uvec2 m_ViewportSize;
        entt::entity m_ActiveCameraEntity;

        void InitScripts();
        void InitPhysics();

        void OnScriptUpdate(double delta);
        void OnPhysicsUpdate(double delta);
        void OnRender(double delta, entt::entity camera);
        void OnRender(double delta, Camera* camera);

        void DestroyScripts();
        void DestroyPhysics();

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
