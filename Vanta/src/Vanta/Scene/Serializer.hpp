#pragma once
#include "Vanta/Scene/Scene.hpp"
#include "Vanta/Util/SerializerUtils.hpp"

namespace Vanta {

    class SceneSerializer {
    public:
        SceneSerializer() = default;
        SceneSerializer(const IO::File& file);

        void Serialize(const Ref<Scene>& scene);
        bool Deserialize(Ref<Scene>& scene);

        template<typename T>
        void Append(const std::string& name, const T& item) {
            YAML::Emitter out;
            out << YAML::Newline;
            out << YAML::BeginMap;
            out << YAML::Key << name << YAML::Value << item;
            out << YAML::EndMap;
            m_File.Append(out.c_str());
        }

        template<typename T>
        Opt<T> Get(const std::string& name) {
            std::string text = m_File.Read();
            YAML::Node data = YAML::Load(text);

            auto node = data[name];
            if (!node)
                return None;
            
            return node.as<T>();
        }

    private:
        IO::File m_File;
    };
}
