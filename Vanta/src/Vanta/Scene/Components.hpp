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

    struct TransformComponent_ {
        glm::vec3 Position = { 0.f, 0.f, 0.f }; // Position
        glm::vec3 Rotation = { 0.f, 0.f, 0.f }; // Rotation in radians
        glm::vec3 Scale    = { 1.f, 1.f, 1.f }; // Scale

        glm::mat4 Transform = glm::mat4(1.f);

        TransformComponent_() = default;
        TransformComponent_(const TransformComponent_& other) = default;
        TransformComponent_(const glm::mat4& transform) {
            SetTransform(transform);
        }
        TransformComponent_(const glm::vec3& position, const glm::vec3& degrees, const glm::vec3& scale) {
            SetTransform(position, degrees, scale);
        }

        void SetTransform(const glm::vec3& position, const glm::vec3& degrees, const glm::vec3& scale) {
            Position = position;
            Rotation = glm::radians(degrees);
            Scale = scale;
            Recalculate();
        }

        void SetTransform(const glm::mat4& transform) {
            Math::Decompose(transform, Position, Rotation, Scale);
            Transform = transform;
        }

        void SetRotationDegrees(const glm::vec3& degrees) { Rotation = glm::radians(degrees); }
        void SetRotationRadians(const glm::vec3& radians) { Rotation = radians; }

        glm::vec3 GetRotationDegrees() const         { return glm::degrees(Rotation); }
        const glm::vec3& GetRotationRadians() const  { return Rotation; }

    private:
        void Recalculate() {
            auto position = glm::translate(glm::mat4(1.f), Position);
            auto rotation = glm::mat4_cast(glm::quat(Rotation));
            auto scale = glm::scale(glm::mat4(1.f), Scale);
            Transform = (glm::mat4)(position * rotation * scale);
        }
    };

    using TransformComponent = Snapshotable<TransformComponent_>;

    struct PhysicsComponent {
        uint Placeholder = 0;
        PhysicsComponent() = default;
        PhysicsComponent(const PhysicsComponent& other) = default;
    };

    struct CameraComponent {
        SceneCamera Camera = SceneCamera::Perspective();
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent& other) = default;
        CameraComponent(const SceneCamera& camera, bool fixedAspectRatio = false)
            : Camera(camera), FixedAspectRatio(fixedAspectRatio) {}
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


    using AllComponents = ComponentList<TransformComponent, PhysicsComponent, CameraComponent, SpriteComponent>;
}
