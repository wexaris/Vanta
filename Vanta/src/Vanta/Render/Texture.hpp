#pragma once

namespace Vanta {

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint slot = 0) const = 0;

        virtual void SetData(const void* data, usize size) = 0;

        virtual uint32 GetWidth() const = 0;
        virtual uint32 GetHeight() const = 0;

        virtual bool operator==(const Texture& other) const = 0;
        virtual bool operator!=(const Texture& other) const = 0;

    protected:
        Texture() = default;
    };

    class Texture2D : public Texture {
    public:
        virtual ~Texture2D() = default;

        static Ref<Texture2D> Create(uint32 width, uint32 height);
        static Ref<Texture2D> Create(const Path& path);
    };

    static inline bool operator==(const Ref<Texture2D>& first, const Ref<Texture2D>& second) {
        return *first.get() == *second.get();
    }
    static inline bool operator!=(const Ref<Texture2D>& first, const Ref<Texture2D>& second) {
        return *first.get() != *second.get();
    }
}
