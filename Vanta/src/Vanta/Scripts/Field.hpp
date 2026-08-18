#pragma once

namespace Vanta {
    namespace Scripts {

        class ScriptInstance;

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

            static ScriptFieldType FromString(std::string_view string);

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

        class ScriptField {
        public:
            std::string Name;
            ScriptFieldType Type;

            ScriptField(std::string name, ScriptFieldType type) : Name(std::move(name)), Type(type) {}

            virtual void GetValue(ScriptInstance* instance, void* buffer) const = 0;
            virtual void SetValue(ScriptInstance* instance, const void* value) const = 0;
        };

        class ScriptFieldInstance {
        public:
            Ref<ScriptField> Field;

            template<typename T>
            T GetFieldValue() const {
                VANTA_CORE_ASSERT(sizeof(T) == m_Size, "Reading wrong type from script field!");
                return *(T*)m_Buffer;
            }

            template<typename T>
            void SetFieldValue(const T& value) {
                VANTA_CORE_ASSERT(sizeof(T) == m_Size, "Writing wrong type to script field!");
                memcpy(m_Buffer, &value, sizeof(T));
            }

            const void* GetFieldData() const {
                return m_Buffer;
            }

        protected:
            ScriptFieldInstance(Ref<ScriptField> field, void* data, usize size)
                : Field(std::move(field)), m_Buffer(data), m_Size(size)
            {
                memset(m_Buffer, 0, m_Size);
            }

        private:
            void* m_Buffer = nullptr;
            const usize m_Size;
        };

        template<typename T>
        class ScriptFieldBuffer : public ScriptFieldInstance {
        public:
            static constexpr usize SIZE = sizeof(T);
            char Buffer[SIZE];

            ScriptFieldBuffer(Ref<ScriptField> field) : ScriptFieldInstance(std::move(field), Buffer, SIZE) {}

            ScriptFieldBuffer(Ref<ScriptField> field, const T& value) : ScriptFieldInstance(std::move(field), Buffer, SIZE) {
                SetFieldValue(value);
            }
        };
    }
}