#include "vantapch.hpp"
#include "Vanta/Project/Serializer.hpp"
#include "Vanta/Util/SerializerUtils.hpp"

#include <yaml-cpp/yaml.h>

namespace Vanta {

    ProjectSerializer::ProjectSerializer(const IO::File& file)
        : m_File(file) {}

    void ProjectSerializer::Serialize(const Ref<Project>& project) {
        YAML::Emitter out;
        out << YAML::BeginMap;

        const auto& config = project->GetConfig();
        out << YAML::Key << "Project" << YAML::Value;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << config.Name;
            out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
            out << YAML::Key << "CacheDirectory" << YAML::Value << config.CacheDirectory.string();
            out << YAML::Key << "ScriptAssemblyPath" << YAML::Value << config.ScriptAssemblyPath.string();
            out << YAML::Key << "InitialScene" << YAML::Value << config.InitialScenePath.string();
            out << YAML::EndMap;
        }

        out << YAML::EndMap;
        m_File.Write(out.c_str());
    }

    bool ProjectSerializer::Deserialize(Ref<Project>& project) {
        std::string text = m_File.Read();

        YAML::Node root;
        try {
            root = YAML::Load(text);
        }
        catch (YAML::ParserException e) {
            VANTA_CORE_ERROR("Failed to parse project file: {}; {}", m_File.Filepath, e.what());
            return false;
        }

        auto data = root["Project"];
        if (!project)
            return false;

        auto& config = project->GetConfig();

        config.Name = data["Name"].as<std::string>();
        VANTA_CORE_TRACE("Deserializing project: {}", config.Name);

        config.AssetDirectory = data["AssetDirectory"].as<std::string>();
        config.CacheDirectory = data["CacheDirectory"].as<std::string>();
        config.ScriptAssemblyPath = data["ScriptAssemblyPath"].as<std::string>();
        config.InitialScenePath = data["InitialScene"].as<std::string>();

        return true;
    }
}
