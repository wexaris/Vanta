#pragma once
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

template<>
struct YAML::convert<Vanta::UUID> {
    static Node encode(const Vanta::UUID& val) {
        Node node;
        node.push_back((Vanta::uint64)val);
        return node;
    }

    static bool decode(const Node& node, Vanta::UUID& val) {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        val = (Vanta::UUID)node[0].as<Vanta::uint64>();
        return true;
    }
};


namespace Vanta {

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& val) {
        out << YAML::Flow;
        out << YAML::BeginSeq << val.x << val.y << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& val) {
        out << YAML::Flow;
        out << YAML::BeginSeq << val.x << val.y << val.z << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& val) {
        out << YAML::Flow;
        out << YAML::BeginSeq << val.x << val.y << val.z << val.w << YAML::EndSeq;
        return out;
    }
}
