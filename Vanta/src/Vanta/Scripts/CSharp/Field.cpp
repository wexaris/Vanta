#include "vantapch.hpp"
#include "Vanta/Scripts/CSharp/Field.hpp"

namespace Vanta {
    namespace CSharp {

        static std::unordered_map<std::string_view, ScriptFieldType> s_MonoFieldTypeMap = {
            { "Bool", ScriptFieldType::Bool },
            { "Char", ScriptFieldType::Char },

            { "Int8", ScriptFieldType::Int8 },
            { "Int16", ScriptFieldType::Int16 },
            { "Int32", ScriptFieldType::Int32 },
            { "Int64", ScriptFieldType::Int64 },

            { "UInt8", ScriptFieldType::UInt8 },
            { "UInt16", ScriptFieldType::UInt16 },
            { "UInt32", ScriptFieldType::UInt32 },
            { "UInt64", ScriptFieldType::UInt64 },

            { "Float", ScriptFieldType::Float },
            { "Double", ScriptFieldType::Double },

            { "Vector2", ScriptFieldType::Vector2 },
            { "Vector3", ScriptFieldType::Vector3 },
            { "Vector4", ScriptFieldType::Vector4 },

            { "Entity", ScriptFieldType::Entity },
        };

        ScriptFieldType::ScriptFieldType(std::string_view string) {
            auto it = s_MonoFieldTypeMap.find(string);
            if (it == s_MonoFieldTypeMap.end()) {
                VANTA_CORE_ERROR("Invalid script field type: {}", string);
                m_Value = ScriptFieldType::None;
            }
            m_Value = it->second;
        }

        const char* ScriptFieldType::ToString() const {
            switch (m_Value) {
            case ScriptFieldType::Bool: return "Bool";
            case ScriptFieldType::Char: return "Char";

            case ScriptFieldType::Int8:  return "Int8";
            case ScriptFieldType::Int16: return "Int16";
            case ScriptFieldType::Int32: return "Int32";
            case ScriptFieldType::Int64: return "Int64";

            case ScriptFieldType::UInt8:  return "UInt8";
            case ScriptFieldType::UInt16: return "UInt16";
            case ScriptFieldType::UInt32: return "UInt32";
            case ScriptFieldType::UInt64: return "UInt64";

            case ScriptFieldType::Float:  return "Float";
            case ScriptFieldType::Double: return "Double";

            case ScriptFieldType::Vector2: return "Vector2";
            case ScriptFieldType::Vector3: return "Vector3";
            case ScriptFieldType::Vector4: return "Vector4";

            case ScriptFieldType::Entity: return "Entity";

            default:
                VANTA_UNREACHABLE("Invalid script field type!");
                return "None";
            }
        }
    }
}
