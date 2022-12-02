#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Render/Renderer2D.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/Scene.hpp"
#include "Vanta/Scene/SceneCamera.hpp"
#include "Vanta/Scripts/ScriptEngine.hpp"

#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

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
        InitPhysics();
        InitScripts();

        // Forward state, so any initial changes made by scripts
        // are propagated to all buffers.
        m_Registry.SwapBuffersFwd();
    }

    void Scene::OnRuntimeEnd() {
        VANTA_PROFILE_FUNCTION();
        DestroyScripts();
        DestroyPhysics();
    }

    void Scene::OnSimulationBegin() {
        VANTA_PROFILE_FUNCTION();
        InitPhysics();
    }

    void Scene::OnSimulationEnd() {
        VANTA_PROFILE_FUNCTION();
        DestroyPhysics();
    }

    void Scene::InitScripts() {
        ScriptEngine::RuntimeBegin(this);

        // Instantiate native scripts
        View<NativeScriptComponent>([&](entt::entity e, NativeScriptComponent& script) {
            script.Create(e, this);
        });

        // Instantiate C# scripts
        View<ScriptComponent>([&](entt::entity e, ScriptComponent& script) {
            script.Create(e, this);
        });

        // Separate creation from OnCreate, because script might try to find another script,
        // which hasn't been initialized yet.
        View<NativeScriptComponent>([&](entt::entity, NativeScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnCreate();
        });

        View<ScriptComponent>([&](entt::entity, ScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnCreate();
        });
    }

    void Scene::DestroyScripts() {
        // Destroy native scripts
        ParallelView<NativeScriptComponent>(m_Barrier, m_Registry,
            [](entt::entity, NativeScriptComponent& script)
        {
            if (script.Instance)
                script.Instance->OnDestroy();
        });

        // Destroy C# scripts
        View<ScriptComponent>([&](entt::entity, ScriptComponent& script) {
            if (script.Instance)
                script.Instance->OnDestroy();
        });

        ParallelView<NativeScriptComponent>(m_Barrier, m_Registry,
            [](entt::entity, NativeScriptComponent& script)
        {
            script.Destroy();
        });

        View<ScriptComponent>([&](entt::entity, ScriptComponent& script) {
            script.Destroy();
        });

        ScriptEngine::RuntimeEnd();
    }

    void Scene::InitPhysics() {
        // Create physics world
        m_PhysicsWorld = new b2World({ 0.f, -9.8f });

        m_Registry.View<TransformComponent, Rigidbody2DComponent>(
            [&](entt::entity e, TransformComponent& tr, Rigidbody2DComponent& rb)
        {
            Entity entity(e, this);

            b2BodyDef bodyDef;
            bodyDef.type = detail::Rigidbody2DTypeToBox2D(rb.Type);
            bodyDef.position = { tr.GetPosition().x, tr.GetPosition().y};
            bodyDef.angle = tr.GetRotationRadians().z;

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb.FixedRotation);

            rb.RuntimeBody = body;

            if (HasComponent<BoxCollider2DComponent>(e)) {
                auto& bc = GetComponent<BoxCollider2DComponent>(e);

                b2PolygonShape shape;
                shape.SetAsBox(bc.Size.x * tr.GetScale().x, bc.Size.y * tr.GetScale().y);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shape;
                fixtureDef.density = bc.Friction;
                fixtureDef.friction = bc.Friction;
                fixtureDef.restitution = bc.Restitution;
                fixtureDef.restitutionThreshold = bc.RestitutionThreshold;

                b2Fixture* fixture = body->CreateFixture(&fixtureDef);

                bc.RuntimeFixture = fixture;
            }

            if (HasComponent<CircleCollider2DComponent>(e)) {
                auto& cc = GetComponent<CircleCollider2DComponent>(e);

                b2CircleShape shape;
                shape.m_p.Set(cc.Offset.x, cc.Offset.y);
                shape.m_radius = cc.Radius;

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shape;
                fixtureDef.density = cc.Friction;
                fixtureDef.friction = cc.Friction;
                fixtureDef.restitution = cc.Restitution;
                fixtureDef.restitutionThreshold = cc.RestitutionThreshold;

                b2Fixture* fixture = body->CreateFixture(&fixtureDef);

                cc.RuntimeFixture = fixture;
            }
        });
    }

    void Scene::DestroyPhysics() {
        delete m_PhysicsWorld;
    }

    void Scene::OnUpdateRuntime(double delta) {
        VANTA_PROFILE_FUNCTION();
        m_Barrier.Wait();
        
        if (!m_IsPaused) {
            m_Registry.SwapBuffers();
            OnScriptUpdate(delta);
            OnPhysicsUpdate(delta);
        }
        else if (m_StepFrames > 0) {
            m_Registry.SwapBuffers();
            OnScriptUpdate(delta);
            OnPhysicsUpdate(delta);
            m_StepFrames--;
        }
        else {
            m_Registry.SwapBuffersFwd();
        }

        OnRender(delta, GetActiveCameraEntity());
    }

    void Scene::OnUpdateSimulation(double delta, Camera* camera) {
        VANTA_PROFILE_FUNCTION();
        m_Barrier.Wait();

        if (!m_IsPaused) {
            m_Registry.SwapBuffers();
            OnPhysicsUpdate(delta);
        }
        else if (m_StepFrames > 0) {
            m_Registry.SwapBuffers();
            OnPhysicsUpdate(delta);
            m_StepFrames--;
        }
        else {
            m_Registry.SwapBuffersFwd();
        }

        OnRender(delta, camera);
    }

    void Scene::OnUpdateEditor(double delta, Camera* camera) {
        VANTA_PROFILE_FUNCTION();
        // Swap via state forwarding.
        // Index swapping causes the buffer state to drift,
        // as the editor overwrites the next state without touching the current one.
        // This makes draw calls flip-flop between two significantly different states,
        // making the model teleport each frame.
        // This shouldn't be a problem in Simulate/Play, since editor interaction isn't expected,
        // and entity state is make consistent by scripts or physics.
        m_Registry.SwapBuffersFwd(); 
        OnRender(delta, camera);
    }

    void Scene::OnScriptUpdate(double delta) {
        VANTA_PROFILE_FUNCTION();

        ParallelView<NativeScriptComponent>(m_Barrier, m_Registry,
            [=](entt::entity, NativeScriptComponent& script)
        {
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

        const uint velocityIterations = 6;
        const uint positionIterations = 2;
        m_PhysicsWorld->Step((float)delta, velocityIterations, positionIterations);

        View<TransformComponent, Rigidbody2DComponent>(
            [&](entt::entity, Buffered<TransformComponent>& buffer, Rigidbody2DComponent& rb)
        {
            b2Body* body = (b2Body*)rb.RuntimeBody;
            auto& position = body->GetPosition();
            float angle = body->GetAngle();

            auto& get = buffer.Get();
            auto& set = buffer.Set();
            set.SetTransformRad({position.x, position.y, get.GetPosition().z}, {0.f, 0.f, angle}, get.GetScale());
        });
    }

    void Scene::OnRender(double delta, entt::entity camera) {
        VANTA_PROFILE_RENDER_FUNCTION();
        if (IsValid(camera)) {
            auto& tr = GetComponent<TransformComponent>(camera).Get();
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
