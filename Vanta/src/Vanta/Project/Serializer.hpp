#pragma once
#include "Vanta/Project/Project.hpp"

namespace Vanta {

    class ProjectSerializer {
    public:
        ProjectSerializer() = default;
        ProjectSerializer(const IO::File& file);

        void Serialize(const Ref<Project>& project);
        bool Deserialize(Ref<Project>& project);

    private:
        IO::File m_File;
    };
}
