#pragma once
#include "Vanta/Render/Camera.hpp"

namespace Vanta {

    class SceneCamera : public Camera {
    public:
        enum class Projection {
            Perspective = 0,
            Orthographic = 1
        };

        virtual ~SceneCamera() = default;

        static SceneCamera Perspective(float fov = 45.f, float nearClip = 0.1f, float farClip = 1000.f);
        static SceneCamera Orthographic(float size = 10.f, float nearClip = -1.f, float farClip = 1.f);

        void SetPerspective(float fov, float nearClip, float farClip);
        void SetOrthographic(float size, float nearClip, float farClip);

        virtual void Resize(uint width, uint height) override;

        void SetPerspectiveFOV(float fov) {
            m_PerspectiveFOV = glm::radians(fov);
            if (m_Projection == Projection::Perspective)
                Recalculate();
        }
        void SetPerspectiveNearClip(float nearClip) {
            m_PerspectiveNear = nearClip;
            if (m_Projection == Projection::Perspective)
                Recalculate();
        }
        void SetPerspectiveFarClip(float farClip) {
            m_PerspectiveFar = farClip;
            if (m_Projection == Projection::Perspective)
                Recalculate();
        }
        float GetPerspectiveFOV() const      { return glm::degrees(m_PerspectiveFOV); }
        float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
        float GetPerspectiveFarClip() const  { return m_PerspectiveFar; }

        void SetOrthographicSize(float size) { m_OrthographicSize = size;
            if (m_Projection == Projection::Orthographic)
            Recalculate();
        }
        void SetOrthographicNearClip(float nearClip) {
            m_OrthographicNear = nearClip;
            if (m_Projection == Projection::Orthographic)
                Recalculate();
        }
        void SetOrthographicFarClip(float farClip) {
            m_OrthographicFar = farClip;
            if (m_Projection == Projection::Orthographic)
                Recalculate();
        }
        float GetOrthographicSize() const     { return m_OrthographicSize; }
        float GetOrthographicNearClip() const { return m_OrthographicNear; }
        float GetOrthographicFarClip() const  { return m_OrthographicFar; }

        void SetProjectionType(Projection type) { m_Projection = type; Recalculate(); }
        Projection GetProjectionType() const    { return m_Projection; }

    private:
        Projection m_Projection = Projection::Perspective;

        float m_PerspectiveFOV;
        float m_PerspectiveNear;
        float m_PerspectiveFar;

        float m_OrthographicSize;
        float m_OrthographicNear;
        float m_OrthographicFar;

        float m_AspectRatio = 16.f / 9.f;

        SceneCamera();

        void Recalculate();
    };
}
