#include <filesystem>
#include <unordered_set>
#include <Vanta/Vanta.hpp>
#include <Vanta/Scripts/Native/Module/Assembly.hpp>
#include <vanta-test-utils/TestHarness.hpp>

using namespace Vanta;

namespace Testing {

    static std::filesystem::path g_ExecutableDirectory;

    static std::filesystem::path GetExecutableDirectory(const char* executablePath) {
        std::error_code errorCode;
        std::filesystem::path path = std::filesystem::absolute(executablePath, errorCode);
        if (!errorCode && path.has_parent_path())
            return path.parent_path();

        return std::filesystem::current_path();
    }

    static std::filesystem::path FindNoComponentUsageAssemblyPath(const std::filesystem::path& executableDirectory) {
        const char* filename = "NoComponentUsageNativeScriptAssembly.dll";

        std::filesystem::path current = executableDirectory;
        std::filesystem::path candidates[] = {
            current / filename,
            current / "bin" / filename,
            current.parent_path() / "bin" / filename,
        };

        for (auto& candidate : candidates) {
            if (std::filesystem::exists(candidate))
                return candidate;
        }

        return candidates[0];
    }

    bool TestNoComponentUsageAssemblyExportsComponents() {
        std::filesystem::path assemblyPath = FindNoComponentUsageAssemblyPath(g_ExecutableDirectory);
        if (!std::filesystem::exists(assemblyPath)) {
            VANTA_CORE_ERROR("Native script assembly not found: {}", assemblyPath.string());
            return false;
        }

        Vanta::Scripts::ScriptAssembly assembly(assemblyPath);
        if (!assembly.IsLoaded())
            return false;

        auto [classes, classCount] = assembly.GetClassList();
        if (classCount != 1)
            return false;
        if (classes == nullptr)
            return false;
        if (std::string(classes[0]) != "NoComponentUsageScript")
            return false;

        auto [fields, fieldCount] = assembly.GetClassFieldList("NoComponentUsageScript");
        if (fieldCount != 1 || fields == nullptr)
            return false;
        if (std::string(fields[0].Name) != "Speed")
            return false;
        if (std::string(fields[0].Type) != "float")
            return false;

        auto [components, componentCount] = assembly.GetComponentList();
        if (components == nullptr)
            return false;

        std::unordered_set<std::string> componentSet;
        for (Vanta::usize i = 0; i < componentCount; i++) {
            componentSet.insert(components[i]);
        }

        if (!componentSet.contains("TransformComponent"))
            return false;
        if (!componentSet.contains("SpriteComponent"))
            return false;
        if (!componentSet.contains("Rigidbody2DComponent"))
            return false;

        if (assembly.GetComponentHash("TransformComponent") == 0)
            return false;
        if (assembly.GetComponentHash("SpriteComponent") == 0)
            return false;
        if (assembly.GetComponentHash("Rigidbody2DComponent") == 0)
            return false;

        return true;
    }
}

using namespace Testing;

int main(int argc, char** argv) {
    Log::Init("Vanta-Tests.log");
    g_ExecutableDirectory = GetExecutableDirectory(argc > 0 ? argv[0] : "");

    TestSet tests("NativeScriptNoComponent", {
        { "NoComponentUsageAssemblyExportsComponents", TestNoComponentUsageAssemblyExportsComponents }
    });

    return tests.IsGood() ? 0 : 1;
}