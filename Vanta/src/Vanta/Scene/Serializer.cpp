#include "vantapch.hpp"
#include "Vanta/Scene/Entity.hpp"
#include "Vanta/Scene/Serializer.hpp"
#include "Vanta/Script/ScriptEngine.hpp"

#include <yaml-cpp/yaml.h>

namespace Vanta {

    SceneSerializer::SceneSerializer(const IO::File& file)
        : m_File(file) {}

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
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "Position" << YAML::Value << component.GetPosition();
            out << YAML::Key << "Rotation" << YAML::Value << component.GetRotationDegrees();
            out << YAML::Key << "Scale" << YAML::Value << component.GetScale();
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

        SerializeComponent<ScriptComponent>(entity, [&out, entity](ScriptComponent& component) {
            out << YAML::Key << "ScriptComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "Class" << YAML::Value << component.ClassName;

            if (!ScriptEngine::ClassExists(component.ClassName))
                return;

            const auto& klass = ScriptEngine::GetClass(component.ClassName);
            if (!klass)
                return;

            const auto& fields = klass->GetFields();

            if (fields.size() > 0) {
                out << YAML::Key << "Fields";
                out << YAML::BeginSeq;

                auto& instances = ScriptEngine::GetFieldInstances(entity);
                for (const auto& [name, field] : fields) {
                    auto it = instances.find(name);
                    if (it == instances.end())
                        continue;

                    auto& instance = it->second;

                    out << YAML::BeginMap; // Field
                    out << YAML::Key << "Name" << YAML::Value << name;
                    out << YAML::Key << "Type" << YAML::Value << field.Type.ToString();
                    out << YAML::Key << "Value" << YAML::Value;

#define WRITE_SCRIPT_FIELD(fieldType, type) \
    case ScriptFieldType::fieldType: { \
        out << (type)instance->GetFieldValue<type>(); \
        break; \
    }

                    switch (field.Type) {
                        WRITE_SCRIPT_FIELD(Bool, bool);
                        WRITE_SCRIPT_FIELD(Char, char);

                        WRITE_SCRIPT_FIELD(Int8,  int8);
                        WRITE_SCRIPT_FIELD(Int16, int16);
                        WRITE_SCRIPT_FIELD(Int32, int32);
                        WRITE_SCRIPT_FIELD(Int64, int64);

                        WRITE_SCRIPT_FIELD(UInt8,  uint8);
                        WRITE_SCRIPT_FIELD(UInt16, uint16);
                        WRITE_SCRIPT_FIELD(UInt32, uint32);
                        WRITE_SCRIPT_FIELD(UInt64, uint64);

                        WRITE_SCRIPT_FIELD(Float,  float);
                        WRITE_SCRIPT_FIELD(Double, double);

                        WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
                        WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
                        WRITE_SCRIPT_FIELD(Vector4, glm::vec4);

                        WRITE_SCRIPT_FIELD(Entity, UUID);
                    }
#undef WRITE_SCRIPT_FIELD

                    out << YAML::EndMap; // Field
                }
                out << YAML::EndSeq; // Field sequence
            }
            out << YAML::EndMap;
        });

        SerializeComponent<SpriteComponent>(entity, [&out](SpriteComponent& component) {
            out << YAML::Key << "SpriteComponent";
            out << YAML::BeginMap;
            if (component.Texture && !component.Texture->GetPath().empty())
                out << YAML::Key << "Texture" << YAML::Value << component.Texture->GetPath().string();
            out << YAML::Key << "TilingFactor" << YAML::Value << component.TilingFactor;
            out << YAML::Key << "Color" << YAML::Value << component.Color;
            out << YAML::EndMap;
        });

        SerializeComponent<CircleRendererComponent>(entity, [&out](CircleRendererComponent& component) {
            out << YAML::Key << "CircleRendererComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "Color" << YAML::Value << component.Color;
            out << YAML::Key << "Thickness" << YAML::Value << component.Thickness;
            out << YAML::Key << "Fade" << YAML::Value << component.Fade;
            out << YAML::EndMap;
        });

        SerializeComponent<Rigidbody2DComponent>(entity, [&out](Rigidbody2DComponent& component) {
            out << YAML::Key << "Rigidbody2DComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "BodyType" << YAML::Value << (int)component.Type;
            out << YAML::Key << "FixedRotation" << YAML::Value << component.FixedRotation;
            out << YAML::EndMap;
        });

        SerializeComponent<BoxCollider2DComponent>(entity, [&out](BoxCollider2DComponent& component) {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "Size" << YAML::Value << component.Size;
            out << YAML::Key << "Offset" << YAML::Value << component.Offset;
            out << YAML::Key << "Density" << YAML::Value << component.Density;
            out << YAML::Key << "Friction" << YAML::Value << component.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << component.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << component.RestitutionThreshold;
            out << YAML::EndMap;
        });

        SerializeComponent<CircleCollider2DComponent>(entity, [&out](CircleCollider2DComponent& component) {
            out << YAML::Key << "CircleCollider2DComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "Radius" << YAML::Value << component.Radius;
            out << YAML::Key << "Offset" << YAML::Value << component.Offset;
            out << YAML::Key << "Density" << YAML::Value << component.Density;
            out << YAML::Key << "Friction" << YAML::Value << component.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << component.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << component.RestitutionThreshold;
            out << YAML::EndMap;
        });

        out << YAML::EndMap;
    }

    void SceneSerializer::Serialize(const Ref<Scene>& scene) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Unnamed"; // TODO: Add names to scenes

        // Serialize active camera
        if (auto activeCamera = scene->GetActiveCameraEntity()) {
            out << YAML::Key << "ActiveCamera" << YAML::Value << activeCamera.GetUUID();
        }

        // Serialize entity list
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        scene->GetRegistry().Each([&](auto entityID) {
            Entity entity(entityID, scene.get());
            if (!entity)
                return;

            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;

        out << YAML::EndMap;
        m_File.Write(out.c_str());
    }

    bool SceneSerializer::Deserialize(const Ref<Scene>& scene) {
        std::string text = m_File.Read();

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

                Entity entity = scene->CreateEntity(name, uuid);

                VANTA_CORE_TRACE("Deserializing entity: {} [{}]", name, uuid);
                
                auto transformComponent = item["TransformComponent"];
                if (transformComponent) {
                    auto& tc = entity.GetComponent<TransformComponent>().Set();
                    auto pos = transformComponent["Position"].as<glm::vec3>();
                    auto rot = transformComponent["Rotation"].as<glm::vec3>();
                    auto scale = transformComponent["Scale"].as<glm::vec3>();
                    tc.SetTransformDeg(pos, rot, scale);
                }
                else {
                    VANTA_CORE_ERROR("Entity missing TransformComponent!");
                    return false;
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

                auto scriptComponent = item["ScriptComponent"];
                if (scriptComponent) {
                    auto& sc = entity.AddComponent<ScriptComponent>();
                    sc.ClassName = scriptComponent["Class"].as<std::string>();

                    auto scriptFields = scriptComponent["Fields"];
                    if (scriptFields) {
                        if (!ScriptEngine::ClassExists(sc.ClassName)) {
                            VANTA_CORE_WARN("Class no longer exists: {}", sc.ClassName);
                            goto after_script_component;
                        }
                        const auto& fields = ScriptEngine::GetClass(sc.ClassName)->GetFields();

                        auto& instances = ScriptEngine::GetFieldInstances(entity);

                        for (auto scriptField : scriptFields) {
                            std::string fieldName = scriptField["Name"].as<std::string>();
                            ScriptFieldType type(scriptField["Type"].as<std::string>());

                            const auto& it = fields.find(fieldName);
                            if (it == fields.end()) {
                                VANTA_CORE_WARN("Field no longer exists: {}", fieldName);
                                continue;
                            }

                            const auto& field = it->second;

#define READ_SCRIPT_FIELD(fieldType, type) \
    case ScriptFieldType::fieldType: { \
        type value = scriptField["Value"].as<type>(); \
        instances[fieldName] = NewBox<ScriptFieldBuffer<type>>(field, value); \
        break; \
    }
                            switch (type) {
                                READ_SCRIPT_FIELD(Bool, bool);
                                READ_SCRIPT_FIELD(Char, char);

                                READ_SCRIPT_FIELD(Int8, int8);
                                READ_SCRIPT_FIELD(Int16, int16);
                                READ_SCRIPT_FIELD(Int32, int32);
                                READ_SCRIPT_FIELD(Int64, int64);

                                READ_SCRIPT_FIELD(UInt8, uint8);
                                READ_SCRIPT_FIELD(UInt16, uint16);
                                READ_SCRIPT_FIELD(UInt32, uint32);
                                READ_SCRIPT_FIELD(UInt64, uint64);

                                READ_SCRIPT_FIELD(Float, float);
                                READ_SCRIPT_FIELD(Double, double);

                                READ_SCRIPT_FIELD(Vector2, glm::vec2);
                                READ_SCRIPT_FIELD(Vector3, glm::vec3);
                                READ_SCRIPT_FIELD(Vector4, glm::vec4);

                                READ_SCRIPT_FIELD(Entity, UUID);
                            }
#undef READ_SCRIPT_FIELD
                        }
                    }
                }
after_script_component:

                auto spriteComponent = item["SpriteComponent"];
                if (spriteComponent) {
                    auto& sp = entity.AddComponent<SpriteComponent>();
                    if (spriteComponent["Texture"])
                        sp.Texture = Texture2D::Create(spriteComponent["Texture"].as<std::string>());
                    sp.TilingFactor = spriteComponent["TilingFactor"].as<float>();
                    sp.Color = spriteComponent["Color"].as<glm::vec4>();
                }

                auto circleRendererComponent = item["CircleRendererComponent"];
                if (circleRendererComponent) {
                    auto& cr = entity.AddComponent<CircleRendererComponent>();
                    cr.Color = circleRendererComponent["Color"].as<glm::vec4>();
                    cr.Thickness = circleRendererComponent["Thickness"].as<float>();
                    cr.Fade = circleRendererComponent["Fade"].as<float>();
                }

                auto rigidbody2DComponent = item["Rigidbody2DComponent"];
                if (rigidbody2DComponent) {
                    auto& rb = entity.AddComponent<Rigidbody2DComponent>();
                    rb.Type = (Rigidbody2DComponent::BodyType)rigidbody2DComponent["BodyType"].as<int>();
                    rb.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
                }

                auto boxCollider2DComponent = item["BoxCollider2DComponent"];
                if (boxCollider2DComponent) {
                    auto& bc = entity.AddComponent<BoxCollider2DComponent>();
                    bc.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
                    bc.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
                    bc.Density = boxCollider2DComponent["Density"].as<float>();
                    bc.Friction = boxCollider2DComponent["Friction"].as<float>();
                    bc.Restitution = boxCollider2DComponent["Restitution"].as<float>();
                    bc.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
                }

                auto circleCollider2DComponent = item["CircleCollider2DComponent"];
                if (circleCollider2DComponent) {
                    auto& cc = entity.AddComponent<CircleCollider2DComponent>();
                    cc.Radius = circleCollider2DComponent["Radius"].as<float>();
                    cc.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
                    cc.Density = circleCollider2DComponent["Density"].as<float>();
                    cc.Friction = circleCollider2DComponent["Friction"].as<float>();
                    cc.Restitution = circleCollider2DComponent["Restitution"].as<float>();
                    cc.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
                }

                // Check for active camera entity
                if (uuid == activeCameraUUID) {
                    scene->SetActiveCameraEntity(entity);
                }
            }
        }

        return true;
    }
}
