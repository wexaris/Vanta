#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Render/Renderer2D.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/NativeScript.hpp"
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
        : m_ViewportSize(Engine::Get().GetWindow().GetWidth(), Engine::Get().GetWindow().GetHeight()) {}

    Scene::~Scene() {
        m_Barrier.Wait();
    }

    Ref<Scene> Scene::Copy(const Ref<Scene>& other) {
        Ref<Scene> scene = NewRef<Scene>();

        scene->m_ViewportSize = other->m_ViewportSize;

        auto idView = other->m_Registry.view<IDComponent>();
        idView.each([&](entt::entity entity, IDComponent& id) {
            Entity oldEntity(entity, other.get());
            Entity newEntity = scene->CreateEntity(id.Name, id.ID);
            detail::CopyComponents(oldEntity, newEntity);
        });

        return scene;
    }

    void Scene::OnRuntimeBegin() {
        VANTA_PROFILE_FUNCTION();
        ParallelView<NativeScriptComponent>(m_Barrier, m_Registry, [&](entt::entity entity, NativeScriptComponent& script) {
            script.Create();
            script.Instance->m_Entity = Entity(entity, this);
            script.Instance->OnCreate();
        });
        // Start Physics
    }

    void Scene::OnRuntimeEnd() {
        VANTA_PROFILE_FUNCTION();
        ParallelView<NativeScriptComponent>(m_Barrier, m_Registry, [](entt::entity, NativeScriptComponent& script) {
            script.Instance->OnDestroy();
            script.Destroy();
        });
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
        m_Barrier.Wait();
        OnScriptUpdate(delta);
        OnPhysicsUpdate(delta);
        OnRender(delta, GetActiveCameraEntity());
    }

    void Scene::OnUpdateSimulation(double delta, Camera* camera) {
        VANTA_PROFILE_FUNCTION();
        m_Barrier.Wait();
        OnPhysicsUpdate(delta);
        OnRender(delta, camera);
    }

    void Scene::OnUpdateEditor(double delta, Camera* camera) {
        VANTA_PROFILE_FUNCTION();
        OnRender(delta, camera);
    }

    void Scene::OnScriptUpdate(double delta) {
        VANTA_PROFILE_FUNCTION();
        ParallelView<NativeScriptComponent>(m_Barrier, m_Registry, [=](entt::entity, NativeScriptComponent& script) {
            script.Instance->OnUpdate(delta);
        });
    }

    void Scene::OnPhysicsUpdate(double) {
        VANTA_PROFILE_FUNCTION();
        ParallelView<TransformComponent, PhysicsComponent>(m_Barrier, m_Registry, [](entt::entity, TransformComponent& tc, PhysicsComponent& pc) {
            tc.GetRealtime().Rotation += pc.Placeholder;
            tc.Snapshot();
        });
    }

    void Scene::OnRender(double delta, Entity camera) {
        VANTA_PROFILE_RENDER_FUNCTION();
        if (camera) {
            auto& tc = camera.GetComponent<TransformComponent>();
            auto& cc = camera.GetComponent<CameraComponent>();
            cc.Camera.SetView(glm::inverse(tc.GetSnapshot().Transform));
            OnRender(delta, &cc.Camera);
        }
    }

    void Scene::OnRender(double, Camera* camera) {
        VANTA_PROFILE_RENDER_FUNCTION();
        if (camera) {
            Renderer2D::SceneBegin(camera);
            LinearView<TransformComponent, SpriteComponent>(m_Registry, [](entt::entity entity, TransformComponent& tc, SpriteComponent& sc) {
                Renderer2D::DrawSprite(tc.GetSnapshot().Transform, sc, (uint32)entity);
            });
            Renderer2D::SceneEnd();
        }
    }

    bool Scene::IsValid(Entity entity) const {
        return m_Registry.valid(entity);
    }

    Entity Scene::CreateEntity(const std::string& name, UUID uuid) {
        VANTA_PROFILE_FUNCTION();
        Entity entity = Entity(m_Registry.create(), this);
        entity.AddComponent<IDComponent>(name, uuid);
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
