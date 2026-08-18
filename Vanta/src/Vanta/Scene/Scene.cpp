#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Render/Renderer2D.hpp"

#include <box2d/box2d.h>

#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/Scene.hpp"
#include "Vanta/Scene/SceneCamera.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/Instance.hpp"
#include "Vanta/Scripts/ScriptManager.hpp"

namespace Vanta {

    namespace detail {
        template<typename... Components>
        static void CopyComponents(Entity from, Entity to) {
            ((from.HasComponent<Components>() ?
                (void)to.AddOrReplaceComponent<Components>(from.GetComponent<Components>()) : void()), ...);
        }

        template<typename... Components>
        static void CopyComponents(ComponentList<Components...>, Entity from, Entity to) {
            CopyComponents<Components...>(from, to);
        }

        static void CopyComponents(Entity from, Entity to) {
            CopyComponents(AllComponents(), from, to);
        }

        static b2BodyType Rigidbody2DTypeToBox2D(Rigidbody2DComponent::BodyType bodyType) {
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

        other->m_Registry.View<IDComponent>([&](entt::entity entity, IDComponent& id) {
            Entity oldEntity(entity, other.get());
            Entity newEntity = scene->CreateEntity(id.Name, id.ID);
            detail::CopyComponents(oldEntity, newEntity);

            if (oldEntity == other->m_ActiveCameraEntity)
                scene->SetActiveCameraEntity(newEntity);
        });

        return scene;
    }

    void Scene::OnRuntimeBegin() {
        VANTA_PROFILE_FUNCTION();
        ResetCommandDiagnostics();
        InitPhysics();
        InitScripts();
    }

    void Scene::OnRuntimeEnd() {
        VANTA_PROFILE_FUNCTION();
        m_Barrier.Wait();
        DestroyScripts();
        DestroyPhysics();
        FlushCommands();
    }

    void Scene::OnSimulationBegin() {
        VANTA_PROFILE_FUNCTION();
        ResetCommandDiagnostics();
        InitPhysics();
    }

    void Scene::OnSimulationEnd() {
        VANTA_PROFILE_FUNCTION();
        DestroyPhysics();
        FlushCommands();
    }

    void Scene::InitScripts() {
        Scripts::ScriptManager::RuntimeBegin(this);

        // Instantiate native scripts
        View<NativeScriptComponent>([&](entt::entity e, NativeScriptComponent& script) {
            script.Create(e, this);
        });

        // Instantiate C# scripts
        View<ScriptComponent>([&](entt::entity e, ScriptComponent& script) {
            script.Create(e, this);
        });

        // Separate script creation from call to OnCreate, because scripts might try to find other scripts,
        // which haven't been initialized yet.
        View<NativeScriptComponent>([](entt::entity, NativeScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnCreate();
        });

        View<ScriptComponent>([](entt::entity, ScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnCreate();
        });
    }

    void Scene::DestroyScripts() {
        // Destroy native scripts
        View<NativeScriptComponent>([](entt::entity, NativeScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnDestroy();
        });

        // Destroy C# scripts
        View<ScriptComponent>([](entt::entity, ScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnDestroy();
        });

        View<NativeScriptComponent>([](entt::entity, NativeScriptComponent& script) {
            script.Destroy();
        });

        View<ScriptComponent>([](entt::entity, ScriptComponent& script) {
            script.Destroy();
        });

        Scripts::ScriptManager::RuntimeEnd();
    }

    void Scene::InitPhysics() {
        // Create physics world
        const float gravity = 9.8f;                 // TODO: Move to a config variable
        const float restitutionThreshold = 0.5f;    // TODO: Move to a config variable
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = { 0.f, -gravity };
        worldDef.restitutionThreshold = restitutionThreshold;

        m_PhysicsWorld = b2CreateWorld(&worldDef);

        m_Registry.View<TransformComponent, Rigidbody2DComponent>(
            [&](entt::entity e, TransformComponent& tr, Rigidbody2DComponent& rb)
        {
            Entity entity(e, this);

            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = detail::Rigidbody2DTypeToBox2D(rb.Type);
            bodyDef.position = { tr.GetPosition().x, tr.GetPosition().y };
            bodyDef.rotation = b2MakeRot(tr.GetRotationRadians().z);

            b2BodyId body = b2CreateBody(m_PhysicsWorld, &bodyDef);
            b2Body_SetFixedRotation(body, rb.FixedRotation);

            rb.RuntimeBody = body;

            if (auto bc = entity.TryGetComponent<BoxCollider2DComponent>()) {
                b2Polygon box = b2MakeBox(bc->Size.x * tr.GetScale().x, bc->Size.y * tr.GetScale().y);

                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.density = bc->Friction;
                shapeDef.material.friction = bc->Friction;
                shapeDef.material.restitution = bc->Restitution;
                shapeDef.material.rollingResistance = bc->RollingResistance;

                bc->RuntimeShape = b2CreatePolygonShape(rb.RuntimeBody, &shapeDef, &box);
            }

            if (auto cc = entity.TryGetComponent<CircleCollider2DComponent>()) {

                b2Circle circle = {};
                circle.center = { cc->Offset.x, cc->Offset.y };
                circle.radius = cc->Radius;

                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.density = cc->Friction;
                shapeDef.material.friction = cc->Friction;
                shapeDef.material.restitution = cc->Restitution;
                shapeDef.material.rollingResistance = cc->RollingResistance;

                cc->RuntimeShape = b2CreateCircleShape(rb.RuntimeBody, &shapeDef, &circle);
            }
        });
    }

    void Scene::DestroyPhysics() {
        b2DestroyWorld(m_PhysicsWorld);
        m_PhysicsWorld = b2_nullWorldId;
    }

    void Scene::OnUpdateRuntime(double delta) {
        VANTA_PROFILE_FUNCTION();
        m_Barrier.Wait();

        if (!m_IsPaused) {
            OnScriptUpdate(delta);
            ApplyCommandsPhase(CommandPhase::Script);
            OnPhysicsUpdate(delta);
            ApplyCommandsPhase(CommandPhase::Physics);
        }
        else if (m_StepFrames > 0) {
            OnScriptUpdate(delta);
            ApplyCommandsPhase(CommandPhase::Script);
            OnPhysicsUpdate(delta);
            ApplyCommandsPhase(CommandPhase::Physics);
            m_StepFrames--;
        }

        OnRender(delta, GetActiveCameraEntity());
    }

    void Scene::OnUpdateSimulation(double delta, Camera* camera) {
        VANTA_PROFILE_FUNCTION();
        m_Barrier.Wait();

        if (!m_IsPaused) {
            OnPhysicsUpdate(delta);
            ApplyCommandsPhase(CommandPhase::Physics);
        }
        else if (m_StepFrames > 0) {
            OnPhysicsUpdate(delta);
            ApplyCommandsPhase(CommandPhase::Physics);
            m_StepFrames--;
        }

        OnRender(delta, camera);
    }

    void Scene::OnUpdateEditor(double delta, Camera* camera) {
        VANTA_PROFILE_FUNCTION();
        ApplyCommandsPhase(CommandPhase::Editor);
        OnRender(delta, camera);
    }

    void Scene::OnScriptUpdate(double delta) {
        VANTA_PROFILE_FUNCTION();

        View<NativeScriptComponent>([=](entt::entity, NativeScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnUpdate((float)delta);
        });

        View<ScriptComponent>([&](entt::entity, ScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnUpdate((float)delta);
        });
    }

    void Scene::OnPhysicsUpdate(double delta) {
        VANTA_PROFILE_FUNCTION();

        const uint subStepCount = 4; // TODO: Move to a config variable
        b2World_Step(m_PhysicsWorld, (float)delta, subStepCount);

        View<TransformComponent, Rigidbody2DComponent>(
            [&](entt::entity e, TransformComponent& tr, Rigidbody2DComponent& rb)
        {
            b2Vec2 position = b2Body_GetPosition(rb.RuntimeBody);
            b2Rot rotation = b2Body_GetRotation(rb.RuntimeBody);
            float angle = b2Rot_GetAngle(rotation);

            EnqueueTransformCommand(SetTransformCommand{
                { e, CommandSource::Physics, CommandPhase::Physics },
                { position.x, position.y, tr.GetPosition().z },
                { 0.f, 0.f, angle },
                tr.GetScale()
            });
        });
    }

    void Scene::OnRender(double delta, entt::entity camera) {
        VANTA_PROFILE_RENDER_FUNCTION();
        if (IsValid(camera)) {
            TransformComponent& tr = GetComponent<TransformComponent>(camera);
            auto& cc = GetComponent<CameraComponent>(camera);
            cc.Camera->SetView(glm::inverse(tr.GetTransform()));
            OnRender(delta, cc.Camera.get());
        }
    }

    void Scene::OnRender(double, Camera* camera) {
        VANTA_PROFILE_RENDER_FUNCTION();
        if (camera) {
            Renderer2D::SceneBegin(camera);

            View<TransformComponent, CircleRendererComponent>([&](entt::entity entity, TransformComponent& tr, CircleRendererComponent& cr) {
                Renderer2D::DrawCircle(tr.GetTransform(), cr.Color, cr.Thickness, cr.Fade, (uint32)entity);
            });

            View<TransformComponent, SpriteComponent>([&](entt::entity entity, TransformComponent& tr, SpriteComponent& sp) {
                Renderer2D::DrawSprite(tr.GetTransform(), sp, (uint32)entity);
            });

            Renderer2D::SceneEnd();
        }
    }

    bool Scene::IsValid(entt::entity entity) const {
        return m_Registry.IsValid(entity);
    }

    Entity Scene::CreateEntity(const std::string& name, UUID uuid) {
        VANTA_PROFILE_FUNCTION();
        Entity entity = Entity(m_Registry.Create(), this);
        AddComponent<IDComponent>(entity, name, uuid);
        AddComponent<TransformComponent>(entity);

        m_EntityMap[uuid] = entity.GetHandle();

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
        auto& id = GetComponent<IDComponent>(entity);
        m_Registry.Destroy(entity);
        m_EntityMap.erase(id.ID);
    }

    Entity Scene::GetEntityByID(UUID uuid) {
        auto iter = m_EntityMap.find(uuid);
        if (iter != m_EntityMap.end())
            return Entity(iter->second, this);
        return Entity();
    }

    Entity Scene::GetEntityByName(std::string_view name) {
        auto view = m_Registry.View<IDComponent>();
        for (auto entity : view) {
            const IDComponent& id = view.get<IDComponent>(entity);
            if (id.Name == name)
                return Entity(entity, this);
        }
        return Entity();
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
        return IsValid(m_ActiveCameraEntity) ?
            Entity(m_ActiveCameraEntity, this) : Entity();
    }

    Camera* Scene::GetActiveCamera() {
        if (!IsValid(m_ActiveCameraEntity))
            return nullptr;

        CameraComponent* cc = m_Registry.TryGetComponent<CameraComponent>(m_ActiveCameraEntity);
        if (!cc)
            return nullptr;

        return cc->Camera.get();
    }
}
