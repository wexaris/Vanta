#include <filesystem>
#include <unordered_set>
#include <Vanta/Vanta.hpp>
#include <Vanta/Scripts/Native/Module/Assembly.hpp>
#include <vanta-test-utils/TestHarness.hpp>

using namespace Vanta;

namespace Testing {

    static std::filesystem::path g_ExecutableDirectory;

    struct ScriptAssemblyExpectations {
        std::unordered_set<std::string> Classes;
        std::unordered_set<std::string> RequiredComponents;
        std::string FieldClass;
        std::string FieldName;
        std::string FieldType;
    };

    static std::filesystem::path GetExecutableDirectory(const char* executablePath) {
        std::error_code errorCode;
        std::filesystem::path path = std::filesystem::absolute(executablePath, errorCode);
        if (!errorCode && path.has_parent_path())
            return path.parent_path();

        return std::filesystem::current_path();
    }

    static std::filesystem::path FindNativeScriptAssemblyPath(const std::filesystem::path& executableDirectory) {
        const char* filename = "NativeScriptTestAssembly.dll";

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

        return current / filename;
    }

    static std::filesystem::path FindFixtureNativeScriptAssemblyPath(const std::filesystem::path& executableDirectory) {
        const char* filename = "Scripts_Native.dll";

        for (std::filesystem::path current = executableDirectory; !current.empty(); current = current.parent_path()) {
            std::filesystem::path candidates[] = {
                current / "Vanta-Tests" / "fixtures" / "NativeScriptBuildFixture" / "build" / "Debug" / "Binaries" / filename,
                current / "Vanta-Tests" / "fixtures" / "NativeScriptBuildFixture" / "build" / "Release" / "Binaries" / filename,
                current / "Vanta-Tests" / "fixtures" / "NativeScriptBuildFixture" / "build" / "RelWithDebInfo" / "Binaries" / filename,
                current / "Vanta-Tests" / "fixtures" / "NativeScriptBuildFixture" / "build" / "MinSizeRel" / "Binaries" / filename,
            };

            for (auto& candidate : candidates) {
                if (std::filesystem::exists(candidate))
                    return std::filesystem::canonical(candidate);
            }
        }

        return executableDirectory / filename;
    }


    static bool ValidateNativeAssembly(
        const std::filesystem::path& assemblyPath,
        const ScriptAssemblyExpectations& expectations)
    {
        if (!std::filesystem::exists(assemblyPath)) {
            VANTA_CORE_ERROR("Native script assembly not found: {}", assemblyPath.string());
            return false;
        }

        Vanta::Native::ScriptAssembly assembly(assemblyPath);
        if (!assembly.IsLoaded())
            return false;

        auto [classes, class_count] = assembly.GetClassList();
        if (class_count != expectations.Classes.size())
            return false;
        if (classes == nullptr)
            return false;

        std::unordered_set<std::string> classSet;
        for (Vanta::usize i = 0; i < class_count; i++) {
            classSet.insert(classes[i]);
        }

        if (classSet != expectations.Classes)
            return false;

        for (const std::string& className : expectations.Classes) {
            Vanta::Native::ClassFunctions* functions = assembly.GetClassFunctions(className.c_str());
            if (functions == nullptr)
                return false;
            if (functions->Constructor == nullptr || functions->OnCreate == nullptr || functions->OnUpdate == nullptr || functions->OnDestroy == nullptr)
                return false;
        }

        for (const std::string& className : expectations.Classes) {
            auto [fields, fieldCount] = assembly.GetClassFieldList(className.c_str());
            if (className == expectations.FieldClass) {
                if (fieldCount != 1 || fields == nullptr)
                    return false;
                if (std::string(fields[0].Name) != expectations.FieldName)
                    return false;
                if (std::string(fields[0].Type) != expectations.FieldType)
                    return false;
            }
            else if (fieldCount != 0) {
                return false;
            }
        }

        auto [components, component_count] = assembly.GetComponentList();
        if (components == nullptr)
            return false;

        std::unordered_set<std::string> componentSet;
        for (Vanta::usize i = 0; i < component_count; i++) {
            componentSet.insert(components[i]);
        }

        for (const std::string& component : expectations.RequiredComponents) {
            if (!componentSet.contains(component))
                return false;
            if (assembly.GetComponentHash(component.c_str()) == 0)
                return false;
        }

        return true;
    }

    bool TestNativeScriptAssembly() {
        ScriptAssemblyExpectations expected = {
            { "TestJumpingEntity", "TestCameraFollower" },
            { "TransformComponent", "SpriteComponent", "Rigidbody2DComponent" },
            "TestCameraFollower",
            "DistanceZ",
            "float"
        };

        return ValidateNativeAssembly(FindNativeScriptAssemblyPath(g_ExecutableDirectory), expected);
    }

    bool TestFixtureNativeScriptAssembly() {
        ScriptAssemblyExpectations expected = {
            { "JumpingEntity", "Camera" },
            { "TransformComponent", "SpriteComponent", "Rigidbody2DComponent" },
            "Camera",
            "DistanceZ",
            "float"
        };

        return ValidateNativeAssembly(FindFixtureNativeScriptAssemblyPath(g_ExecutableDirectory), expected);
    }

}

using namespace Testing;

int main(int argc, char** argv) {
    Log::Init("Vanta-Tests.log");
    g_ExecutableDirectory = GetExecutableDirectory(argc > 0 ? argv[0] : "");

    TestSet testNativeScript("NativeScript", {
        { "NativeScriptAssembly", TestNativeScriptAssembly },
        { "FixtureNativeScriptAssembly", TestFixtureNativeScriptAssembly }
    });
    return testNativeScript.IsGood() ? 0 : 1;
}