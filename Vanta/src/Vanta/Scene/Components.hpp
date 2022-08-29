#pragma once
#include "Vanta/Render/Texture.hpp"
#include "Vanta/Scene/SceneCamera.hpp"
#include "Vanta/Scene/Buffer.hpp"

namespace Vanta {

    struct IDComponent {
        std::string Name;
        IDComponent(const IDComponent& other) = default;
        IDComponent(const std::string& name) : Name(name) {}
    };

    struct TransformComponent {
        glm::vec3 Position = { 0.f, 0.f, 0.f }; // Position
        glm::vec3 Rotation = { 0.f, 0.f, 0.f }; // Rotation in radians
        glm::vec3 Scale    = { 1.f, 1.f, 1.f }; // Scale

        glm::mat4 Transform = glm::mat4(1.f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent& other) = default;
        TransformComponent(const glm::mat4& transform) {
            SetTransform(transform);
        }
        TransformComponent(const glm::vec3& position, const glm::vec3& degrees, const glm::vec3& scale) {
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


    /// ///////////////// BUFFERED COMPONENTS /////////////////////////////////
    
#define BUFFER_COUNT 2

    VANTA_COMPONENT_BUFFER(TransformComponent, BUFFER_COUNT);

#undef BUFFER_COUNT
}
