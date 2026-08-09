#include <vanta-test-utils/CoreTestsCommon.hpp>

namespace Testing {

    bool TestScriptProjectScaffolding() {
        std::filesystem::path root = std::filesystem::temp_directory_path() / "vanta-project-scaffolding";
        std::filesystem::remove_all(root);
        std::filesystem::create_directories(root);

        auto project = Project::New(root);
        if (!project)
            return false;

        const std::filesystem::path csharpDir = root / "Scripts" / "CSharp";
        const std::filesystem::path nativeDir = root / "Scripts" / "Native";

        const bool hasCSharpCMake = std::filesystem::exists(csharpDir / "CMakeLists.txt");
        const bool hasCSharpCsproj = std::filesystem::exists(csharpDir / "Scripts_CSharp.csproj");
        const bool hasNativeCMake = std::filesystem::exists(nativeDir / "CMakeLists.txt");
        const bool noBatFiles = !std::filesystem::exists(csharpDir / "Build.bat") && !std::filesystem::exists(nativeDir / "Build.bat");

        std::filesystem::remove_all(root);
        return hasCSharpCMake && hasCSharpCsproj && hasNativeCMake && noBatFiles;
    }
}
