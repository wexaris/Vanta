#include "vantapch.hpp"
#include "Vanta/Core/Engine.hpp"
#include "Vanta/Script/ScriptEngine.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Vanta {

    namespace detail {
        MonoAssembly* LoadCSAssembly(const std::string& assemblyPath) {
            auto data = IO::File(assemblyPath).ReadBinary();

            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(data.data(), data.size(), 1, &status, 0);

            if (status != MONO_IMAGE_OK) {
                const char* error = mono_image_strerror(status);
                VANTA_CORE_CRITICAL("Failed to load C# assembly: {}", error);
                return nullptr;
            }

            MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
            mono_image_close(image);

            return assembly;
        }

        void PrintAssemblyTypes(MonoAssembly* assembly) {
            MonoImage* image = mono_assembly_get_image(assembly);
            const MonoTableInfo* typeDefs = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
            int32 numTypes = mono_table_info_get_rows(typeDefs);

            for (int32 i = 0; i < numTypes; i++) {
                uint32 cols[MONO_TYPEDEF_SIZE];
                mono_metadata_decode_row(typeDefs, i, cols, MONO_TYPEDEF_SIZE);

                const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
                const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

                VANTA_CORE_INFO("{}.{}", nameSpace, name);
            }
        }
    }

    struct ScriptData {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;
        MonoAssembly* CoreAssembly = nullptr;
    };

    ScriptData s_Data;

    void ScriptEngine::Init() {
        InitMono();
    }

    void ScriptEngine::Shutdown() {
        ShutdownMono();
    }

    void ScriptEngine::InitMono() {
        mono_set_assemblies_path("mono/4.5");

        s_Data.RootDomain = mono_jit_init("VantaJIT");
        VANTA_CORE_ASSERT(s_Data.RootDomain, "Failed to initialize Mono JIT runtime!");

        s_Data.AppDomain = mono_domain_create_appdomain(const_cast<char*>("VantaScripts"), nullptr);
        mono_domain_set(s_Data.AppDomain, true);

        auto assembly = Engine::Get().ScriptDirectory() / "Vanta-Script.dll";
        s_Data.CoreAssembly = detail::LoadCSAssembly(assembly.string());
        detail::PrintAssemblyTypes(s_Data.CoreAssembly);

        MonoImage* asmImage = mono_assembly_get_image(s_Data.CoreAssembly);

        MonoClass* monoClass = mono_class_from_name(asmImage, "Vanta", "Main");

        MonoObject* instance = mono_object_new(s_Data.AppDomain, monoClass);
        mono_runtime_object_init(instance);

        

        float value = 3;
        void* param = &value;
        void* params[] = { &value };

        MonoMethod* method_1 = mono_class_get_method_from_name(monoClass, "IncrementFloatVar", 1);
        mono_runtime_invoke(method_1, instance, &param, nullptr);

        MonoMethod* method_2 = mono_class_get_method_from_name(monoClass, "PrintFloatVar", 0);
        mono_runtime_invoke(method_2, instance, nullptr, nullptr);
    }

    void ScriptEngine::ShutdownMono() {
        s_Data.AppDomain = nullptr;

        mono_jit_cleanup(s_Data.RootDomain);
        s_Data.RootDomain = nullptr;
    }
}
