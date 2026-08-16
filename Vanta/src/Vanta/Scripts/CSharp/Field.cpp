#include "vantapch.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/CSharp/Field.hpp"

#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>

namespace Vanta {
    namespace CSharp {

        namespace {
            static std::unordered_map<std::string, ScriptFieldType> s_MonoFieldTypeMap = {
                { "System.Boolean", ScriptFieldType::Bool },
                { "System.Char", ScriptFieldType::Char },

                { "System.Byte", ScriptFieldType::Int8 },
                { "System.Int16", ScriptFieldType::Int16 },
                { "System.Int32", ScriptFieldType::Int32 },
                { "System.Int64", ScriptFieldType::Int64 },

                { "System.UByte", ScriptFieldType::UInt8 },
                { "System.UInt16", ScriptFieldType::UInt16 },
                { "System.UInt32", ScriptFieldType::UInt32 },
                { "System.UInt64", ScriptFieldType::UInt64 },

                { "System.Single", ScriptFieldType::Float },
                { "System.Double", ScriptFieldType::Double },

                { "Vanta.Vector2", ScriptFieldType::Vector2 },
                { "Vanta.Vector3", ScriptFieldType::Vector3 },
                { "Vanta.Vector4", ScriptFieldType::Vector4 },

                { "Vanta.Entity", ScriptFieldType::Entity },
            };

            static ScriptFieldType MonoTypeToFieldType(MonoType* type) {
                std::string name = mono_type_get_name(type);
                auto it = s_MonoFieldTypeMap.find(name);
                if (it == s_MonoFieldTypeMap.end()) {
                    VANTA_CORE_ERROR("Invalid script field type: {}", name);
                    return ScriptFieldType::None;
                }
                return it->second;
            }
        }

        CSharpScriptField::CSharpScriptField(std::string name, MonoType* type, MonoClassField* field)
            : ScriptField(std::move(name), MonoTypeToFieldType(type)), MonoField(field)
        {
        }

        CSharpScriptField::CSharpScriptField(std::string name, ScriptFieldType type, MonoClassField* field)
            : ScriptField(std::move(name), type), MonoField(field)
        {
        }

        void CSharpScriptField::GetValue(ScriptInstance* instance, void* buffer) const {
            VANTA_CORE_ASSERT(instance, "Script object instance cannot be null!");
            VANTA_CORE_ASSERT(buffer, "Buffer cannot be null!");
            VANTA_CORE_ASSERT(MonoField, "MonoField cannot be null!");
            mono_field_get_value((MonoObject*)instance->GetRuntimeInstance(), MonoField, buffer);
        }

        void CSharpScriptField::SetValue(ScriptInstance* instance, const void* value) const {
            VANTA_CORE_ASSERT(instance, "Script object instance cannot be null!");
            VANTA_CORE_ASSERT(value, "Value cannot be null!");
            VANTA_CORE_ASSERT(MonoField, "MonoField cannot be null!");
            mono_field_set_value((MonoObject*)instance->GetRuntimeInstance(), MonoField, const_cast<void*>(value));
        }

    }
}
