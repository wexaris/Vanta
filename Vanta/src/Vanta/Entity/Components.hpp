#pragma once
#include "Vanta/Entity/Buffer.hpp"
#include "Vanta/Render/Camera.hpp"
#include "Vanta/Render/Texture.hpp"

#include "Vanta/Render/Renderer2D.hpp"

namespace Vanta {

    struct IDComponent {
        std::string Name;
        IDComponent(const IDComponent& other) = default;
        IDComponent(const std::string& name) : Name(name) {}
    };

    struct TransformComponent {
        glm::vec3 Position = { 0.f, 0.f, 0.f };
        glm::vec3 Rotation = { 0.f, 0.f, 0.f };
        glm::vec3 Scale    = { 1.f, 1.f, 1.f };

        glm::mat4 Transform = glm::mat4(1.f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent& other) = default;
        TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
            : Position(position), Rotation(rotation), Scale(scale)
        {
            Recalculate();
        }

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
        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent& other) = default;
        SpriteComponent(const Ref<Texture2D>& texture)
            : Texture(texture) {}

        void Render(double /*delta*/, const glm::mat4& transform) {
            Renderer2D::DrawQuad(transform, {0.8, 0.3, 0.6, 1.0});
        }
    };

    using AllComponents = ComponentList<TransformComponent, PhysicsComponent, CameraComponent, SpriteComponent>;


    /// ///////////////// BUFFERED COMPONENTS /////////////////////////////////
    
#define BUFFER_COUNT 2

    VANTA_COMPONENT_BUFFER(TransformComponent, BUFFER_COUNT);

#undef BUFFER_COUNT
}
