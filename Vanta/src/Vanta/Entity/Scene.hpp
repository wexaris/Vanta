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

        template<typename... Components>
        auto GetAllEntitiesWith() { return m_Registry.view<Components...>(); }

        void OnWindowResize(WindowResizeEvent& e);

        void SetActiveCamera(CameraComponent& camera) { m_ActiveCamera = camera.Camera; }
        Camera& GetActiveCamera()                     { return *m_ActiveCamera.get(); }

        entt::registry& GetRegistry() { return m_Registry; }

    private:
        entt::registry m_Registry;
        Ref<Camera> m_ActiveCamera = nullptr;
        glm::uvec2 m_ViewportSize;

        Buffer<TransformComponentList> m_DataBuffer;

        void OnScriptUpdate(double delta);
        void OnPhysicsUpdate(double delta);
        void OnRender(double delta);

        template<typename T>
        void OnComponentAdded(Entity& /*entity*/, T& /*component*/) {}

        template<>
        void OnComponentAdded<CameraComponent>(Entity&, CameraComponent& component) {
            // Set active camera, if one's not set
            if (!m_ActiveCamera)
                m_ActiveCamera = component.Camera;

            component.Camera->Resize(m_ViewportSize.x, m_ViewportSize.y);
        }

        friend class Entity;
    };
}
