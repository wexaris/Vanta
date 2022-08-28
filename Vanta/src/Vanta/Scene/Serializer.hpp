#pragma once
#include "Vanta/Scene/Scene.hpp"

namespace Vanta {

    class SceneSerializer {
    public:
        SceneSerializer() = default;
        SceneSerializer(const Ref<Scene>& scene);

        void Serialize(const Path& filepath);
        bool Deserialize(const Path& filepath);

    private:
        Ref<Scene> m_Scene;
    };
}
