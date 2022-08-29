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
                (void)to.AddOrReplaceComponent<Components>(from.GetComponent<Components>()) : void()), ...);
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
        VANTA_PROFILE_FUNCTION();
        TransformComponentBuffers::Setup(m_Registry);
    }

    Scene::~Scene() {}

    Ref<Scene> Scene::Copy(const Ref<Scene>& other) {
        Ref<Scene> scene = NewRef<Scene>();

        scene->m_ViewportSize = other->m_ViewportSize;

        auto idView = other->m_Registry.view<IDComponent>();
        idView.each([&](entt::entity entity, IDComponent& id) {
            Entity oldEntity(entity, other.get());

            //const auto& uuid = id.ID;
            const auto& name = id.Name;
            Entity newEntity = scene->CreateEntity(name);

            detail::CopyComponents(oldEntity, newEntity);
        });

        return scene;
    }

    void Scene::OnRuntimeBegin() {
        VANTA_PROFILE_FUNCTION();
        // Start Physics
    }

    void Scene::OnRuntimeEnd() {
        VANTA_PROFILE_FUNCTION();
        // Stop Physics
    }

    void Scene::OnSimulationBegin() {
        VANTA_PROFILE_FUNCTION();
        // Start Physics
    }

    void Scene::OnSimulationEnd() {
        VANTA_PROFILE_FUNCTION();
        // Stop Physics
    }

    void Scene::OnUpdateRuntime(double delta) {
        VANTA_PROFILE_FUNCTION();
        OnScriptUpdate(delta);
        OnPhysicsUpdate(delta);
        OnRender(delta, GetActiveCamera());

        m_UpdateBarrier.Wait();
        m_DataBuffer.Next();
    }

    void Scene::OnUpdateSimulation(double delta, Camera* camera) {
        VANTA_PROFILE_FUNCTION();
        OnPhysicsUpdate(delta);
        OnRender(delta, camera);

        m_UpdateBarrier.Wait();
        m_DataBuffer.Next();
    }

    void Scene::OnUpdateEditor(double delta, Camera* camera) {
        VANTA_PROFILE_FUNCTION();
        OnRender(delta, camera);
    }

    void Scene::OnScriptUpdate(double) {
        VANTA_PROFILE_FUNCTION();
        // TODO: Update scripts
    }

    struct PhysicsUpdate {
        double Delta;
        PhysicsUpdate(double delta) : Delta(delta) {}

        void operator()(entt::entity, TransformComponent& old_tr, TransformComponent& tr, PhysicsComponent&) const {
            VANTA_PROFILE_SCOPE("Physics Update");
            tr = old_tr;
        }
    };

    struct CameraUpdate {
        double Delta;
        CameraUpdate(double delta) : Delta(delta) {}

        void operator()(entt::entity, TransformComponent& tr, CameraComponent& camera) const {
            camera.Camera.SetView(glm::inverse(tr.Transform));
        }
    };

    void Scene::OnPhysicsUpdate(double delta) {
        VANTA_PROFILE_FUNCTION();
        m_DataBuffer.View<PhysicsComponent>(m_Registry, ParallelDispatch<PhysicsUpdate>(m_UpdateBarrier, delta));
        //m_DataBuffer.View<PhysicsComponent>(m_Registry, LinearDispatch<PhysicsUpdate>(delta));

        m_DataBuffer.ViewCurr<CameraComponent>(m_Registry, LinearDispatch<CameraUpdate>(delta));
    }

    struct Render {
        double Delta;
        Render(double delta) : Delta(delta) {}

        void operator()(entt::entity entity, TransformComponent& tr, SpriteComponent& sp) const {
            Renderer2D::DrawSprite(tr.Transform, sp, (int)entity);
        }
    };

    void Scene::OnRender(double delta, Camera* camera) {
        VANTA_PROFILE_RENDER_FUNCTION();
        if (camera) {
            Renderer2D::SceneBegin(camera);
            m_DataBuffer.ViewCurr<SpriteComponent>(m_Registry, LinearDispatch<Render>(delta));
            Renderer2D::SceneEnd();
        }
    }

    bool Scene::IsValid(Entity entity) const {
        return m_Registry.valid(entity);
    }

    Entity Scene::CreateEntity(const std::string& name/*, UUID id*/) {
        VANTA_PROFILE_FUNCTION();
        Entity entity = Entity(m_Registry.create(), this);
        entity.AddComponent<IDComponent>(name/*, id*/);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    Entity Scene::DuplicateEntity(Entity entity) {
        VANTA_PROFILE_FUNCTION();
        Entity newEntity = CreateEntity(entity.GetName());
        detail::CopyComponents(entity, newEntity);
        return newEntity;
    }

    void Scene::DestroyEntity(Entity entity) {
        VANTA_PROFILE_FUNCTION();
        m_Registry.destroy(entity);
    }

    void Scene::OnViewportResize(uint width, uint height) {
        VANTA_PROFILE_FUNCTION();
        m_ViewportSize.x = width;
        m_ViewportSize.y = height;
        if (auto camera = GetActiveCamera())
            camera->Resize(width, height);
    }

    void Scene::SetActiveCameraEntity(Entity& camera) {
        m_ActiveCameraEntity = camera.operator entt::entity();
    }

    Entity Scene::GetActiveCameraEntity() {
        return m_Registry.valid(m_ActiveCameraEntity) ?
            Entity(m_ActiveCameraEntity, this) : Entity();
    }

    Camera* Scene::GetActiveCamera() {
        if (m_Registry.valid(m_ActiveCameraEntity))
            if (auto comp = m_Registry.try_get<CameraComponent>(m_ActiveCameraEntity))
                return &comp->Camera;
        return nullptr;
    }
}
