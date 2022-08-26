#pragma once

namespace Vanta {

    class Camera {
    public:
        virtual ~Camera() = default;

        virtual void Resize(uint width, uint height) = 0;

        virtual void SetTransform(const glm::vec3& position, const glm::vec3& rotation);
        virtual void SetTransform(const glm::mat4& transform);

        virtual void SetView(const glm::mat4& transform);

        virtual const glm::mat4& GetView() const           { return m_ViewMatrix; }
        virtual const glm::mat4& GetProjection() const     { return m_ProjectionMatrix; }
        virtual const glm::mat4& GetViewProjection() const { return m_ViewProjectionMatrix; }

    protected:
        glm::mat4 m_ViewMatrix = glm::mat4(1.f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
        glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.f);

        Camera() = default;
    };

    struct OrthographicCameraBounds {
        float Left, Right, Top, Bottom;

        OrthographicCameraBounds(float left, float right, float bottom, float top)
            : Left(left), Right(right), Top(top), Bottom(bottom) {}

        float GetWidth() const { return Right - Left; }
        float GetHeight() const { return Top - Bottom; }
    };

    class OrthographicCamera : public Camera {
    public:
        OrthographicCamera();

        void Resize(uint width, uint height) override;

        void SetZoom(float zoom);
        float GetZoom() const { return m_Zoom; }

        const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }

    private:
        float m_Zoom = 1.f;
        float m_AspectRatio;
        OrthographicCameraBounds m_Bounds;

        void Recalculate();
    };

    class PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera();

        void Resize(uint width, uint height) override;

        void SetFOV(float fow);
        float GetFOV() const { return m_FOV; }

    private:
        float m_FOV = 45.f;
        float m_AspectRatio;

        void Recalculate();
    };
}
