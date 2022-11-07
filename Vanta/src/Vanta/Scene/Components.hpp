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

    struct IDComponent {
        UUID ID;
        std::string Name;
        IDComponent(const IDComponent& other) = default;
        IDComponent(const std::string& name, UUID uuid) : ID(uuid), Name(name) {}
    };

    struct TransformComponent {
        glm::mat4 Transform = glm::mat4(1.f);

        glm::vec3 Position = { 0.f, 0.f, 0.f }; // Position
        glm::vec3 Rotation = { 0.f, 0.f, 0.f }; // Rotation in radians
        glm::vec3 Scale    = { 1.f, 1.f, 1.f }; // Scale

        TransformComponent() = default;
        TransformComponent(const TransformComponent& other) = default;
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
            Recalculate();
        }

        void SetTransformDeg(const glm::vec3& position, const glm::vec3& degrees, const glm::vec3& scale) {
            SetTransformRad(position, glm::radians(degrees), scale);
        }

        void SetTransform(const glm::mat4& transform) {
            Transform = transform;
            Math::Decompose(Transform, Position, Rotation, Scale);
        }

        void SetRotationRad(const glm::vec3& radians) { Rotation = radians; }
        void SetRotationDeg(const glm::vec3& degrees) { Rotation = glm::radians(degrees); }

        const glm::vec3& GetRotationRadians() const  { return Rotation; }
        glm::vec3 GetRotationDegrees() const         { return glm::degrees(Rotation); }

    private:
        void Recalculate() {
            auto position = glm::translate(glm::mat4(1.f), Position);
            auto rotation = glm::mat4_cast(glm::quat(Rotation));
            auto scale = glm::scale(glm::mat4(1.f), Scale);
            Transform = (glm::mat4)(position * rotation * scale);
        }
    };

    struct CameraComponent {
        SceneCamera Camera = SceneCamera::Perspective();
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent& other) = default;
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
        Rigidbody2DComponent(const Rigidbody2DComponent& other) = default;
    };

    struct BoxCollider2DComponent {
        glm::vec2 Size   = { 0.5f, 0.5f };
        glm::vec2 Offset = { 0.0f, 0.0f };

        float Density = 1.0f;
        float Friction = 1.0f;
        float Restitution = 0.5f;           // bounciness
        float RestitutionThreshold = 0.5f;

        // Physics runtime instance
        void* RuntimeFixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent& other) = default;
    };

    struct CircleCollider2DComponent {
        float Radius = 1.0f;
        glm::vec2 Offset = { 0.0f, 0.0f };

        float Density = 1.0f;
        float Friction = 1.0f;
        float Restitution = 0.5f;           // bounciness
        float RestitutionThreshold = 0.5f;

        // Physics runtime instance
        void* RuntimeFixture = nullptr;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent& other) = default;
    };

    struct SpriteComponent {
        Ref<Texture2D> Texture = nullptr;
        float TilingFactor = 1.f;
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };

        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent& other) = default;
        SpriteComponent(const glm::vec4& color)
            : SpriteComponent(nullptr, color) {}
        SpriteComponent(const Ref<Texture2D>& texture, const glm::vec4& tint)
            : Texture(texture), Color(tint) {}
    };

    class NativeScript; // Forward declare

    struct NativeScriptComponent {
        NativeScript* Instance;

        NativeScript* (*CreateInstance)();
        void (*DestroyInstance)(NativeScript*);

        NativeScriptComponent() = default;

        void Create()  { Instance = CreateInstance(); }
        void Destroy() { DestroyInstance(Instance); Instance = nullptr; }

        template<typename T> requires IsBase_v<NativeScript, T>
        void Bind() {
            CreateInstance = []() { return static_cast<NativeScript*>(new T()); };
            DestroyInstance = [](NativeScript* instance) { delete instance; };
        }
    };


    using AllComponents = ComponentList<TransformComponent, CameraComponent,
        Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, SpriteComponent, NativeScriptComponent>;
}
