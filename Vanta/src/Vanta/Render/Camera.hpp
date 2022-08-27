#pragma once

namespace Vanta {

    class Camera {
    public:
        Camera() = default;
        virtual ~Camera() = default;

        virtual void Resize(uint width, uint height);

        virtual void SetTransform(const glm::mat4& transform);
        virtual void SetTransform(const glm::vec3& position, const glm::vec3& rotation);

        virtual void SetPosition(const glm::vec3& position);
        virtual void SetRotation(const glm::vec3& rotation);

        virtual void Move(const glm::vec3& offset);
        virtual void Rotate(const glm::vec3& offset);

        virtual const glm::mat4& GetTransform() {
            if (m_DirtyTransform)
                RecalculateTransform();
            return m_Transform;
        }
        virtual const glm::vec3& GetPosition() const  { return m_Position; }
        virtual glm::vec3 GetRotationDeg() const { return glm::degrees(m_Rotation); }
        virtual glm::vec3 GetRotationRad() const { return m_Rotation; }

        virtual void SetView(const glm::mat4& view);
        virtual void SetProjection(const glm::mat4& proj);

        virtual const glm::mat4& GetView() const           { return m_ViewMatrix; }
        virtual const glm::mat4& GetProjection() const     { return m_ProjectionMatrix; }
        virtual const glm::mat4& GetViewProjection() {
            if (m_DirtyTransform) {
                RecalculateTransform();
            }
            else if (m_DirtyViewProjection) {
                RecalculateViewProjection();
            }
            return m_ViewProjectionMatrix;
        }

    protected:
        glm::vec3 m_Position = { 0.f, 0.f, 0.f };
        glm::vec3 m_Rotation = { 0.f, 0.f, 0.f };
        glm::mat4 m_Transform = glm::mat4(1.f);
        bool m_DirtyTransform = false;

        glm::mat4 m_ViewMatrix = glm::inverse(m_Transform);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
        glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.f);
        bool m_DirtyViewProjection = false;

        virtual void RecalculateTransform();
        virtual void RecalculateViewProjection();
    };
}
