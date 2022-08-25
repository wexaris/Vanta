#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Render/Renderer2D.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/Scene.hpp"

namespace Vanta {

    namespace detail {
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
    }

    Scene::Scene()
        : m_ViewportSize(Engine::Get().GetWindow().GetWidth(), Engine::Get().GetWindow().GetHeight())
    {
        TransformComponentBuffers::Setup(m_Registry);
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
        m_DataBuffer.Next();
        OnRender(delta, m_ActiveCamera.get());
    }

    void Scene::OnUpdateSimulation(double delta, Camera* camera) {
        OnPhysicsUpdate(delta);
        m_DataBuffer.Next();
        OnRender(delta, camera);
    }

    void Scene::OnUpdateEditor(double delta, Camera* camera) {
        m_DataBuffer.Next();
        OnRender(delta, camera);
    }

    void Scene::OnScriptUpdate(double) {
        // TODO: Update scripts
    }

    struct PhysicsUpdate {
        double Delta;
        PhysicsUpdate(double delta) : Delta(delta) {}

        void operator()(entt::entity, TransformComponent& old_tr, TransformComponent& tr, PhysicsComponent&) const {
            tr = old_tr;
        }
    };

    void Scene::OnPhysicsUpdate(double delta) {
        m_DataBuffer.View<PhysicsComponent>(m_Registry, LinearDispatch<PhysicsUpdate>(delta));
    }

    struct CameraUpdate {
        double Delta;
        CameraUpdate(double delta) : Delta(delta) {}

        void operator()(entt::entity, TransformComponent& tr, CameraComponent& camera) const {
            camera.Camera->SetTransform(tr.Transform);
        }
    };

    struct Render {
        double Delta;
        Render(double delta) : Delta(delta) {}

        void operator()(entt::entity, TransformComponent& tr, SpriteComponent& sp) const {
            sp.Render(Delta, tr.Transform);
        }
    };

    void Scene::OnRender(double delta, Camera* camera) {
        if (camera) {
            m_DataBuffer.ViewPrev<CameraComponent>(m_Registry, LinearDispatch<CameraUpdate>(delta));

            Renderer2D::SceneBegin(camera);
            m_DataBuffer.ViewPrev<SpriteComponent>(m_Registry, LinearDispatch<Render>(delta));
            Renderer2D::SceneEnd();
        }
        m_DataBuffer.Next();
    }

    bool Scene::IsValid(Entity entity) const {
        return m_Registry.valid(entity);
    }

    Entity Scene::CreateEntity(const std::string& name/*, UUID id*/) {
        Entity entity = Entity(m_Registry.create(), this);
        entity.AddComponent<IDComponent>(name/*, id*/);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    Entity Scene::DuplicateEntity(Entity entity) {
        Entity newEntity = CreateEntity(entity.GetName());
        detail::CopyComponents(entity, newEntity);
        return newEntity;
    }

    void Scene::DestroyEntity(Entity entity) {
        m_Registry.destroy(entity);
    }

    void Scene::OnViewportResize(uint width, uint height) {
        m_ViewportSize.x = width;
        m_ViewportSize.y = height;
        if (m_ActiveCamera) {
            GetActiveCamera()->Resize(width, height);
        }
    }

    void Scene::SetActiveCameraEntity(Entity& camera) {
        m_ActiveCameraEntity = camera.operator entt::entity();
    }

    Entity Scene::GetActiveCameraEntity() {
        return m_ActiveCameraEntity ? Entity(m_ActiveCameraEntity.value(), this) : Entity();
    }
}
