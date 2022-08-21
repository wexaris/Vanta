#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Entity/Entity.hpp"
#include "Vanta/Entity/Scene.hpp"
#include "Vanta/Render/Renderer2D.hpp"

namespace Vanta {

    Scene::Scene() : m_ViewportSize(Engine::Get().GetWindow().GetWidth(), Engine::Get().GetWindow().GetHeight()) {
        Buffer<TransformComponentList>::InitBuffers(m_Registry);
    }

    Scene::~Scene() {}

    void Scene::OnRuntimeBegin() {
        // Start Physics
    }

    void Scene::OnRuntimeEnd() {
        // Stop Physics
    }

    void Scene::OnSimulationBegin() {
        // Start Physics
    }

    void Scene::OnSimulationEnd() {
        // Stop Physics
    }

    void Scene::OnUpdateRuntime(double delta) {
        OnScriptUpdate(delta);
        OnPhysicsUpdate(delta);
        OnRender(delta);
    }

    void Scene::OnUpdateSimulation(double delta) {
        OnPhysicsUpdate(delta);
        OnRender(delta);
    }

    void Scene::OnUpdateEditor(double delta) {
        OnRender(delta);
    }

    void Scene::OnScriptUpdate(double /*delta*/) {
        // TODO: Update scripts
    }

    struct PhysicsUpdate {
        double Delta;
        PhysicsUpdate(double delta) : Delta(delta) {}

        void operator()(entt::entity, TransformComponent&, PhysicsComponent&) const {
            // TODO: Do physics
        }
    };

    void Scene::OnPhysicsUpdate(double delta) {
        m_DataBuffer.View<PhysicsComponent>(m_Registry, ParalelDispatch<PhysicsUpdate>(delta));
        m_DataBuffer.Next();
    }

    struct Render {
        double Delta;
        Render(double delta) : Delta(delta) {}

        void operator()(entt::entity, TransformComponent& tr, SpriteComponent& sp) const {
            sp.Render(Delta, tr.Transform);
        }
    };

    void Scene::OnRender(double delta) {
        if (m_ActiveCamera) {
            Renderer2D::SceneBegin(m_ActiveCamera.get());
            m_DataBuffer.View<SpriteComponent>(m_Registry, LinearDispatch<Render>(delta));
            Renderer2D::SceneEnd();
        }
        m_DataBuffer.Next();
    }

    template<typename... Components>
    void CopyComponents(Entity from, Entity to) {
        ((from.HasComponent<Components>() ?
            (void)to.AddComponent<Components>(from.GetComponent<Components>()) : void()), ...);
    }

    template<typename... Components>
    void CopyComponents(ComponentList<Components...>, Entity from, Entity to) {
        CopyComponents<Components...>(from, to);
    }

    void CopyComponents(Entity from, Entity to) {
        CopyComponents(AllComponents(), from, to);
    }

    Entity Scene::CreateEntity(const std::string& name/*, UUID id*/) {
        Entity entity = Entity(m_Registry.create(), this);
        entity.AddComponent<IDComponent>(name/*, id*/);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    Entity Scene::DuplicateEntity(Entity entity) {
        Entity newEntity = CreateEntity(entity.GetName());
        CopyComponents(entity, newEntity);
        return newEntity;
    }

    void Scene::DestroyEntity(Entity entity) {
        m_Registry.destroy(entity);
    }

    void Scene::OnWindowResize(WindowResizeEvent& e) {
        m_ViewportSize.x = e.Width;
        m_ViewportSize.y = e.Height;
        m_ActiveCamera->Resize(e.Width, e.Height);
    }
}
