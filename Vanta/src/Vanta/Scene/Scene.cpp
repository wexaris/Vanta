#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Render/Renderer2D.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/NativeScript.hpp"
#include "Vanta/Scene/Scene.hpp"

#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

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

        b2BodyType Rigidbody2DTypeToBox2D(Rigidbody2DComponent::BodyType bodyType) {
            switch (bodyType) {
            case Rigidbody2DComponent::BodyType::Static: return b2_staticBody;
            case Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
            case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
            default:
                VANTA_UNREACHABLE("Invalid Rigidbody2D body type!");
                return b2_staticBody;
            }
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

    void Scene::OnRender(double delta, entt::entity camera) {
        VANTA_PROFILE_RENDER_FUNCTION();
        if (IsValid(camera)) {
            auto& tr = GetComponent<TransformComponent>(camera);
            auto& cc = GetComponent<CameraComponent>(camera);
            cc.Camera.SetView(glm::inverse(tr.Transform));
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

    bool Scene::IsValid(entt::entity entity) const {
        return m_Registry.valid(entity);
    }

    Entity Scene::CreateEntity(const std::string& name, UUID uuid) {
        VANTA_PROFILE_FUNCTION();
        Entity entity = Entity(m_Registry.create(), this);
        AddComponent<IDComponent>(entity, name, uuid);
        AddComponent<TransformComponent>(entity);
        return entity;
    }

    Entity Scene::DuplicateEntity(entt::entity entity) {
        VANTA_PROFILE_FUNCTION();
        Entity e(entity, this);
        Entity newEntity = CreateEntity(e.GetName());
        detail::CopyComponents(e, newEntity);
        return newEntity;
    }

    void Scene::DestroyEntity(entt::entity entity) {
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

    void Scene::SetActiveCameraEntity(entt::entity camera) {
        m_ActiveCameraEntity = camera;
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
