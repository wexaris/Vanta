#pragma once
#include "Vanta/Render/Texture.hpp"
#include "Vanta/Scene/SceneCamera.hpp"

#include <entt/entt.hpp>

namespace Vanta {

    /// <summary>
    /// Helper type for grouping and shuttling components around.
    /// </summary>
    template<typename... Components>
    using ComponentList = entt::type_list<Components...>;

    template<usize N, typename... Types>
    struct Get<N, ComponentList<Types...>> : Get<N, Types...> {};

    template <typename T, typename... Types>
    struct Contains<T, ComponentList<Types...>> : Contains<Types...> {};

    /// <summary>
    /// Wrapper for components that need to be buffered.
    /// Provides a realtime and snapshot copy of the given component.
    /// </summary>
    template<typename Component>
    class Snapshotable {
    public:
        Component& GetRealtime()             { return m_Data; }
        const Component& GetSnapshot() const { return m_Snapshot; }

        void Snapshot() { std::swap(m_Snapshot, m_Data); }

    private:
        Component m_Data;
        Component m_Snapshot;
    };


    /// ///////////////// COMPONENTS //////////////////////////////////////////

    class Scene;

    struct IDComponent {
        UUID ID;
        std::string Name;
        IDComponent(const IDComponent&) = default;
        IDComponent(const std::string& name, UUID uuid) : ID(uuid), Name(name) {}
    };

    struct TransformComponent {
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;

        TransformComponent(const glm::mat4& transform) {
            SetTransform(transform);
        }

        TransformComponent(const glm::vec3& position, const glm::vec3& degrees, const glm::vec3& scale) {
            SetTransformDeg(position, degrees, scale);
        }

        void SetTransformRad(const glm::vec3& position, const glm::vec3& radians, const glm::vec3& scale) {
            Position = position;
            Rotation = radians;
            Scale = scale;
            DirtyTransform = true;
        }

        void SetTransformDeg(const glm::vec3& position, const glm::vec3& degrees, const glm::vec3& scale) {
            SetTransformRad(position, glm::radians(degrees), scale);
        }

        void SetTransform(const glm::mat4& transform) {
            Transform = transform;
            Math::Decompose(Transform, Position, Rotation, Scale);
        }

        void SetPosition(const glm::vec3& position) {
            Position = position;
            DirtyTransform = true;
        }

        void SetRotationDeg(const glm::vec3& degrees) {
            SetRotationRad(glm::radians(degrees));
        }

        void SetRotationRad(const glm::vec3& radians) {
            Rotation = radians;
            DirtyTransform = true;
        }

        void SetScale(const glm::vec3& scale) {
            Scale = scale;
            DirtyTransform = true;
        }

        const glm::mat4& GetTransform() {
            if (DirtyTransform)
                Recalculate();
            return Transform;
        }

        const glm::vec3& GetPosition() const { return Position; }

        const glm::vec3& GetRotationRadians() const { return Rotation; }
        glm::vec3 GetRotationDegrees() const        { return glm::degrees(Rotation); }

        const glm::vec3& GetScale() const { return Scale; }

    private:
        glm::mat4 Transform = glm::mat4(1.f);

        glm::vec3 Position = { 0.f, 0.f, 0.f }; // Position
        glm::vec3 Rotation = { 0.f, 0.f, 0.f }; // Rotation in radians
        glm::vec3 Scale = { 1.f, 1.f, 1.f };    // Scale

        bool DirtyTransform = false;

        void Recalculate() {
            auto position = glm::translate(glm::mat4(1.f), Position);
            auto rotation = glm::mat4_cast(glm::quat(Rotation));
            auto scale = glm::scale(glm::mat4(1.f), Scale);
            Transform = (glm::mat4)(position * rotation * scale);
            DirtyTransform = false;
        }
    };

    struct CameraComponent {
        SceneCamera Camera = SceneCamera::Perspective();
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(const SceneCamera& camera, bool fixedAspectRatio = false)
            : Camera(camera), FixedAspectRatio(fixedAspectRatio) {}
    };

    struct Rigidbody2DComponent {
        enum class BodyType { Static = 0, Dynamic, Kinematic };

        BodyType Type = BodyType::Static;
        bool FixedRotation = false;

        // Physics runtime instance
        void* RuntimeBody = nullptr;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    };

    struct BoxCollider2DComponent {
        glm::vec2 Size   = { 0.5f, 0.5f };
        glm::vec2 Offset = { 0.0f, 0.0f };

        float Density = 1.0f;
        float Friction = 0.4f;
        float Restitution = 0.3f;           // bounciness
        float RestitutionThreshold = 0.5f;

        // Physics runtime instance
        void* RuntimeFixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };

    struct CircleCollider2DComponent {
        float Radius = 0.5f;
        glm::vec2 Offset = { 0.0f, 0.0f };

        float Density = 1.0f;
        float Friction = 0.4f;
        float Restitution = 0.2f;           // bounciness
        float RestitutionThreshold = 0.5f;

        // Physics runtime instance
        void* RuntimeFixture = nullptr;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
    };

    struct SpriteComponent {
        Ref<Texture2D> Texture = nullptr;
        float TilingFactor = 1.f;
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };

        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent&) = default;
        SpriteComponent(const glm::vec4& color)
            : SpriteComponent(nullptr, color) {}
        SpriteComponent(const Ref<Texture2D>& texture, const glm::vec4& tint)
            : Texture(texture), Color(tint) {}
    };

    struct CircleRendererComponent {
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
        float Thickness = 1.f;
        float Fade = 0.01f;
    };

    class ScriptInstance; // Forward declare

    struct ScriptComponent {
        std::string ClassName;

        // Script runtime instance
        Ref<ScriptInstance> Instance = nullptr;

        void Create(entt::entity e, Scene* scene);
        void Destroy();

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
    };

    class NativeScript; // Forward declare

    struct NativeScriptComponent {
        // Script runtime instance
        NativeScript* Instance;

        NativeScript* (*CreateInstance)();
        void (*DestroyInstance)(NativeScript*);

        NativeScriptComponent() = default;
        NativeScriptComponent(const NativeScriptComponent&) = default;

        void Create(entt::entity e, Scene* scene);
        void Destroy();

        template<typename T> requires IsBase_v<NativeScript, T>
        void Bind() {
            CreateInstance = []() { return static_cast<NativeScript*>(new T()); };
            DestroyInstance = [](NativeScript* instance) { delete instance; };
        }
    };

    using AllComponents = ComponentList<TransformComponent, CameraComponent,
        Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent,
        SpriteComponent, CircleRendererComponent, ScriptComponent, NativeScriptComponent>;
}
