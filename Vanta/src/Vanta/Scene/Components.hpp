#pragma once
#include "Vanta/Render/Camera.hpp"
#include "Vanta/Render/Texture.hpp"
#include "Vanta/Scene/Buffer.hpp"

namespace Vanta {

    struct IDComponent {
        std::string Name;
        IDComponent(const IDComponent& other) = default;
        IDComponent(const std::string& name) : Name(name) {}
    };

    struct TransformComponent {
        TransformComponent() = default;
        TransformComponent(const TransformComponent& other) = default;
        TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
            : m_Position(position), m_Rotation(rotation), m_Scale(scale)
        {
            Recalculate();
        }

        void SetTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
            m_Position = position;
            m_Rotation = rotation;
            m_Scale = scale;
            Recalculate();
        }

        void SetTransform(const glm::mat4& transform) { m_Transform = transform; }
        const glm::mat4& GetTransform()               { return m_Transform; }

    private:
        glm::vec3 m_Position = { 0.f, 0.f, 0.f };
        glm::vec3 m_Rotation = { 0.f, 0.f, 0.f };
        glm::vec3 m_Scale = { 1.f, 1.f, 1.f };

        glm::mat4 m_Transform = glm::mat4(1.f);

        void Recalculate() {
            auto position = glm::translate(glm::mat4(1.f), m_Position);
            auto rotation = glm::mat4_cast(glm::quat(m_Rotation));
            auto scale = glm::scale(glm::mat4(1.f), m_Scale);
            m_Transform = (glm::mat4)(position * rotation * scale);
        }
    };

    struct PhysicsComponent {
        uint Placeholder = 0;
        PhysicsComponent() = default;
        PhysicsComponent(const PhysicsComponent& other) = default;
    };

    struct CameraComponent {
    private:
        using Camera_t = Camera; // Allow member variable to be called `Camera`

    public:
        Ref<Camera_t> Camera;
        CameraComponent() = default;
        CameraComponent(const CameraComponent& other) = default;
        CameraComponent(const Ref<Camera_t>& camera)
            : Camera(camera) {}
    };

    struct SpriteComponent {
        Ref<Texture2D> Texture;
        glm::vec4 Tint = { 1.f, 1.f, 1.f, 1.f };

        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent& other) = default;
        SpriteComponent(const glm::vec4& tint)
            : SpriteComponent(nullptr, tint) {}
        SpriteComponent(const Ref<Texture2D>& texture, const glm::vec4& tint)
            : Texture(texture), Tint(tint) {}

        void Render(double delta, const glm::mat4& transform);
    };

    using AllComponents = ComponentList<TransformComponent, PhysicsComponent, CameraComponent, SpriteComponent>;


    /// ///////////////// BUFFERED COMPONENTS /////////////////////////////////
    
#define BUFFER_COUNT 2

    VANTA_COMPONENT_BUFFER(TransformComponent, BUFFER_COUNT);

#undef BUFFER_COUNT
}
