#pragma once

namespace Vanta {

    /// <summary>
    /// Non-owning data buffer.
    /// </summary>
    struct Buffer {
    public:
        Buffer() = default;
        Buffer(const Buffer&) = default;

        Buffer(usize size) {
            Allocate(size);
        }

        static Buffer Copy(Buffer other) {
            Buffer result(other.m_Size);
            memcpy(result.m_Data, other.m_Data, other.m_Size);
            return result;
        }

        void Allocate(usize size) {
            Destroy();
            m_Data = new uint8[size];
            m_Size = size;
        }

        void Destroy() {
            if (m_Data)
                delete[] m_Data;

            m_Data = nullptr;
            m_Size = 0;
        }

        uint8* Data() { return m_Data; }
        usize Size()  { return m_Size; }

        template<typename T>
        T* As() {
            return (T*)m_Data;
        }

        operator bool() const { return (bool)m_Data; }

    private:
        uint8* m_Data = nullptr;
        usize m_Size = 0;
    };

    struct ScopedBuffer {
    public:
        ScopedBuffer(Buffer buffer) : m_Buffer(buffer) {}

        ScopedBuffer(usize size) : m_Buffer(size) {}

        ~ScopedBuffer() {
            m_Buffer.Destroy();
        }

        uint8* Data() { return m_Buffer.Data(); }
        usize Size()  { return m_Buffer.Size(); }

        template<typename T>
        T* As() {
            return m_Buffer.As<T>();
        }

        operator bool() const { return (bool)m_Buffer; }

    private:
        Buffer m_Buffer;
    };
}
