#pragma once
#include "Vanta/Render/Camera.hpp"

namespace Vanta {

    class SceneCamera : public Camera {
    public:
        enum class Projection {
            Perspective = 0,
            Orthographic = 1
        };

        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetPerspective(float fov, float nearClip, float farClip);
        void SetOrthographic(float size, float nearClip, float farClip);

        virtual void Resize(uint width, uint height) override;

        void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; Recalculate(); }
        float GetPerspectiveVerticalFOV() const           { return m_PerspectiveFOV; }
        void SetPerspectiveNearClip(float nearClip)       { m_PerspectiveNear = nearClip; Recalculate(); }
        float GetPerspectiveNearClip() const              { return m_PerspectiveNear; }
        void SetPerspectiveFarClip(float farClip)         { m_PerspectiveFar = farClip; Recalculate(); }
        float GetPerspectiveFarClip() const               { return m_PerspectiveFar; }

        void SetOrthographicSize(float size)         { m_OrthographicSize = size; Recalculate(); }
        float GetOrthographicSize() const            { return m_OrthographicSize; }
        void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; Recalculate(); }
        float GetOrthographicNearClip() const        { return m_OrthographicNear; }
        void SetOrthographicFarClip(float farClip)   { m_OrthographicFar = farClip; Recalculate(); }
        float GetOrthographicFarClip() const         { return m_OrthographicFar; }

        void SetProjectionType(Projection type) { m_Projection = type; Recalculate(); }
        Projection GetProjectionType() const    { return m_Projection; }

    private:
        Projection m_Projection = Projection::Perspective;

        float m_PerspectiveFOV = glm::radians(45.f);
        float m_PerspectiveNear = 0.1f;
        float m_PerspectiveFar = 1000.f;

        float m_OrthographicSize = 10.f;
        float m_OrthographicNear = -1.f;
        float m_OrthographicFar = 1.0f;

        float m_AspectRatio = 16.f / 9.f;

        void Recalculate();
    };
}
