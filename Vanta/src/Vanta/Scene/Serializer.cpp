#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/Serializer.hpp"

#include <yaml-cpp/yaml.h>

template<>
struct YAML::convert<glm::vec2> {
    static Node encode(const glm::vec2& val) {
        Node node;
        node.push_back(val.x);
        node.push_back(val.y);
        return node;
    }

    static bool decode(const Node& node, glm::vec2& val) {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        val.x = node[0].as<float>();
        val.y = node[1].as<float>();
        return true;
    }
};

template<>
struct YAML::convert<glm::vec3> {
    static Node encode(const glm::vec3& val) {
        Node node;
        node.push_back(val.x);
        node.push_back(val.y);
        node.push_back(val.z);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& val) {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        val.x = node[0].as<float>();
        val.y = node[1].as<float>();
        val.z = node[2].as<float>();
        return true;
    }
};

template<>
struct YAML::convert<glm::vec4> {
    static Node encode(const glm::vec4& val) {
        Node node;
        node.push_back(val.x);
        node.push_back(val.y);
        node.push_back(val.z);
        node.push_back(val.w);
        return node;
    }

    static bool decode(const Node& node, glm::vec4& val) {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        val.x = node[0].as<float>();
        val.y = node[1].as<float>();
        val.z = node[2].as<float>();
        val.w = node[3].as<float>();
        return true;
    }
};

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& val) {
    out << YAML::Flow;
    out << YAML::BeginSeq << val.x << val.y << val.z << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& val) {
    out << YAML::Flow;
    out << YAML::BeginSeq << val.x << val.y << val.z << val.w << YAML::EndSeq;
    return out;
}

namespace Vanta {

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
        : m_Scene(scene) {}

    template<typename Component, typename Func>
    static void SerializeComponent(Entity entity, Func&& func) {
        if (entity.HasComponent<Component>()) {
            func(entity.GetComponent<Component>());
        }
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity) {
        VANTA_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entity missing ID component!");

        out << YAML::BeginMap;

        auto& idComponent = entity.GetComponent<IDComponent>();

        out << YAML::Key << "Entity" << YAML::Value;
        out << YAML::Flow << YAML::BeginSeq << idComponent.Name << idComponent.ID << YAML::EndSeq;

        SerializeComponent<TransformComponent>(entity, [&out](TransformComponent& component) {
            auto& comp = component.GetSnapshot();
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "Position" << YAML::Value << comp.Position;
            out << YAML::Key << "Rotation" << YAML::Value << comp.GetRotationDegrees();
            out << YAML::Key << "Scale" << YAML::Value << comp.Scale;
            out << YAML::EndMap;
        });

        SerializeComponent<PhysicsComponent>(entity, [&out](PhysicsComponent& component) {
            out << YAML::Key << "PhysicsComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "Placeholder" << YAML::Value << component.Placeholder;
            out << YAML::EndMap;
        });

        SerializeComponent<CameraComponent>(entity, [&out](CameraComponent& component) {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;
            
            out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)component.Camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << component.Camera.GetPerspectiveFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << component.Camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << component.Camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << component.Camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << component.Camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << component.Camera.GetOrthographicFarClip();
            out << YAML::EndMap;

            out << YAML::Key << "FixedAspectRatio" << YAML::Value << component.FixedAspectRatio;
            out << YAML::EndMap;
        });

        SerializeComponent<SpriteComponent>(entity, [&out](SpriteComponent& component) {
            out << YAML::Key << "SpriteComponent";
            out << YAML::BeginMap;
            //out << YAML::Key << "Texture" << YAML::Value << component.Texture;
            out << YAML::Key << "TilingFactor" << YAML::Value << component.TilingFactor;
            out << YAML::Key << "Color" << YAML::Value << component.Color;
            out << YAML::EndMap;
        });

        out << YAML::EndMap;
    }

    void SceneSerializer::Serialize(const IO::File& file) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Unnamed"; // TODO: Add names to scenes

        // Serialize active camera
        if (auto activeCamera = m_Scene->GetActiveCameraEntity()) {
            out << YAML::Key << "ActiveCamera" << YAML::Value << activeCamera.GetUUID();
        }

        // Serialize entity list
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_Scene->GetRegistry().each([&](auto entityID) {
            Entity entity(entityID, m_Scene.get());
            if (!entity)
                return;

            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;

        out << YAML::EndMap;
        file.Write(out.c_str());
    }

    bool SceneSerializer::Deserialize(const IO::File& file) {
        std::string text = file.Read();

        YAML::Node data = YAML::Load(text);
        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        VANTA_CORE_TRACE("Deserializing scene: {}", sceneName);

        // UUID of active camera
        auto activeCamera = data["ActiveCamera"];
        UUID activeCameraUUID = activeCamera ? UUID(activeCamera.as<uint64>()) : UUID();

        // Entity list
        auto entities = data["Entities"];
        if (entities) {
            for (auto item : entities) {
                auto entityNode = item["Entity"];
                std::string name = entityNode[0].as<std::string>();
                UUID uuid = entityNode[1].as<uint64>();

                Entity entity = m_Scene->CreateEntity(name, uuid);

                VANTA_CORE_TRACE("Deserializing entity: {} [{}]", name, uuid);

                // Check for active camera entity
                if (uuid == activeCameraUUID) {
                    m_Scene->SetActiveCameraEntity(entity);
                }
                
                auto transformComponent = item["TransformComponent"];
                if (transformComponent) {
                    auto& snap = entity.GetComponent<TransformComponent>();
                    auto& tc = snap.GetRealtime();
                    auto pos = transformComponent["Position"].as<glm::vec3>();
                    auto rot = transformComponent["Rotation"].as<glm::vec3>();
                    auto scale = transformComponent["Scale"].as<glm::vec3>();
                    tc.SetTransform(pos, rot, scale);
                    snap.Snapshot();
                }
                else {
                    VANTA_CORE_ERROR("Entity missing TransformComponent!");
                    return false;
                }

                auto physicsComponent = item["PhysicsComponent"];
                if (physicsComponent) {
                    auto& pc = entity.AddComponent<PhysicsComponent>();
                    pc.Placeholder = physicsComponent["Placeholder"].as<uint>();
                }

                auto cameraComponent = item["CameraComponent"];
                if (cameraComponent) {
                    auto& cc = entity.AddComponent<CameraComponent>();

                    auto camera = cameraComponent["Camera"];
                    cc.Camera.SetProjectionType((SceneCamera::Projection)camera["ProjectionType"].as<int>());

                    cc.Camera.SetPerspectiveFOV(camera["PerspectiveFOV"].as<float>());
                    cc.Camera.SetPerspectiveNearClip(camera["PerspectiveNear"].as<float>());
                    cc.Camera.SetPerspectiveFarClip(camera["PerspectiveFar"].as<float>());

                    cc.Camera.SetOrthographicSize(camera["OrthographicSize"].as<float>());
                    cc.Camera.SetOrthographicNearClip(camera["OrthographicNear"].as<float>());
                    cc.Camera.SetOrthographicFarClip(camera["OrthographicFar"].as<float>());

                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
                }

                auto spriteComponent = item["SpriteComponent"];
                if (spriteComponent) {
                    auto& sc = entity.AddComponent<SpriteComponent>();
                    //pc.Texture = spriteComponent["Texture"].as<std::string>();
                    sc.TilingFactor = spriteComponent["TilingFactor"].as<float>();
                    sc.Color = spriteComponent["Color"].as<glm::vec4>();
                }
            }
        }

        return true;
    }
}
