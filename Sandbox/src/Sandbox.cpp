#include <Vanta/Vanta.hpp>
#include <Vanta/EntryPoint.hpp>

class TestLayer : public Vanta::Layer {
public:
    TestLayer() : Layer("TestLayer") {
        VANTA_PROFILE_FUNCTION();
        auto camera_e = m_ActiveScene.CreateEntity("Camera");
        camera_e.AddComponent<Vanta::CameraComponent>();

        auto sprite_e = m_ActiveScene.CreateEntity("Sprite");
        sprite_e.AddComponent<Vanta::PhysicsComponent>();
        sprite_e.AddComponent<Vanta::SpriteComponent>(glm::vec4{ 0.3f, 0.2f, 0.6f, 1.f });

        m_ActiveScene.OnRuntimeBegin();
    }

    ~TestLayer() {
        VANTA_PROFILE_FUNCTION();

        m_ActiveScene.OnRuntimeEnd();
    }

    void OnUpdate(double delta) override {
        VANTA_PROFILE_FUNCTION();

        Vanta::RenderCommand::Clear({ 0.1f, 0.1f, 0.1f, 0.1f });

        constexpr float radius = 10.f;
        float camX = sin((float)Vanta::Duration::SinceLaunch().AsSecondsf()) * radius;
        float camY = cos((float)Vanta::Duration::SinceLaunch().AsSecondsf()) * radius;
        auto view = glm::lookAt(glm::vec3(camX, camY, 5), glm::vec3(0, 0, 0), glm::vec3(0.f, 1.f, 0.f));
        auto& tc = m_ActiveScene.GetActiveCameraEntity().GetComponent<Vanta::TransformComponent>();
        tc.GetRealtime().SetTransform(glm::inverse(view));
        tc.Snapshot();

        m_ActiveScene.OnUpdateRuntime(delta);
    }

    void OnEvent(Vanta::Event& e) override {
        VANTA_PROFILE_FUNCTION();

        Vanta::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Vanta::WindowResizeEvent>(EVENT_METHOD(TestLayer::OnResize));
    }

    bool OnResize(Vanta::WindowResizeEvent& e) {
        m_ActiveScene.OnViewportResize(e.Width, e.Height);
        return false;
    }

private:
    Vanta::Scene m_ActiveScene;
};

class SandboxEngine : public Vanta::Engine {
public:
    SandboxEngine(const Vanta::EngineParams& params) : Engine(params) {
        PushLayer(new TestLayer());
    }
};

namespace Vanta {

    Engine* CreateEngine() {
        EngineParams params;
        params.Window.Height = 800;
        params.Window.Title = "Sandbox";
        return new SandboxEngine(params);
    }
}
