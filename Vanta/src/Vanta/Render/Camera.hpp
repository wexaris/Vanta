#pragma once

namespace Vanta {

    class Camera {
    public:
        Camera() = default;
        virtual ~Camera() = default;

        virtual void Resize(uint width, uint height) = 0;

        void SetView(const glm::mat4& view);
        void SetProjection(const glm::mat4& proj);

        const glm::mat4& GetView() const           { return m_ViewMatrix; }
        const glm::mat4& GetProjection() const     { return m_ProjectionMatrix; }
        virtual const glm::mat4& GetViewProjection() {
            if (m_DirtyViewProjection) RecalculateViewProjection();
            return m_ViewProjectionMatrix;
        }

    private:
        glm::mat4 m_ViewMatrix = glm::inverse(glm::mat4(1.f));
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
        glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.f);
        bool m_DirtyViewProjection = false;

        void RecalculateViewProjection();
    };
}
