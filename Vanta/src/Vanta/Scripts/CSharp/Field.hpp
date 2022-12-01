#pragma once

extern "C" {
    typedef struct _MonoClassField MonoClassField;
}

namespace Vanta {
    namespace CSharp {

        class ScriptFieldType {
        public:
            enum Value {
                None = 0,
                Bool, Char,
                Int8, Int16, Int32, Int64,
                UInt8, UInt16, UInt32, UInt64,
                Float, Double,
                Vector2, Vector3, Vector4,
                Entity,
            };

            ScriptFieldType() = default;
            constexpr ScriptFieldType(Value value) : m_Value(value) {}
            ScriptFieldType(std::string_view string);

            constexpr bool operator==(ScriptFieldType other) const { return m_Value == other.m_Value; }
            constexpr bool operator!=(ScriptFieldType other) const { return m_Value != other.m_Value; }

#if 1 // Allow usage: switch(value)
            constexpr operator Value() const { return m_Value; }
#endif

            // Prevent usage: if(value)
            explicit operator bool() const = delete;

            const char* ToString() const;

        private:
            Value m_Value;
        };

        struct ScriptField {
            std::string Name;
            ScriptFieldType Type;

            MonoClassField* MonoField;
        };

        struct ScriptFieldInstance {
            ScriptField Field;

            template<typename T>
            T GetFieldValue() {
                VANTA_CORE_ASSERT(sizeof(T) == m_Size, "Reading wrong type from script field!");
                return *(T*)m_Buffer;
            }

            template<typename T>
            void SetFieldValue(const T& value) {
                VANTA_CORE_ASSERT(sizeof(T) == m_Size, "Writing wrong type to script field!");
                memcpy(m_Buffer, &value, sizeof(T));
            }

        protected:
            ScriptFieldInstance(ScriptField field, void* data, usize size)
                : Field(field), m_Buffer(data), m_Size(size)
            {
                memset(m_Buffer, 0, m_Size);
            }

        private:
            friend class ScriptEngine;

            void* m_Buffer = nullptr;
            const usize m_Size;
        };

        template<typename T>
        struct ScriptFieldBuffer : public ScriptFieldInstance {
            static constexpr usize SIZE = sizeof(T);
            char Buffer[SIZE];

            ScriptFieldBuffer(ScriptField field) : ScriptFieldInstance(field, Buffer, SIZE) {}

            ScriptFieldBuffer(ScriptField field, const T& value) : ScriptFieldInstance(field, Buffer, SIZE) {
                SetFieldValue(value);
            }
        };
    }
}
