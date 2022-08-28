#pragma once
#include "Vanta/Scene/Scene.hpp"

namespace Vanta {

    class SceneSerializer {
    public:
        SceneSerializer() = default;
        SceneSerializer(const Ref<Scene>& scene);

        void Serialize(const IO::File& file);
        bool Deserialize(const IO::File& file);

    private:
        Ref<Scene> m_Scene;
    };
}
