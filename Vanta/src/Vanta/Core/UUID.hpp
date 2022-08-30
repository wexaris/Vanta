#pragma once

namespace Vanta {

    class UUID {
    public:
        UUID();
        UUID(uint64 uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }

    private:
        uint64 m_UUID;
    };
}

namespace std {
    template <typename T> struct hash;

    template<>
    struct hash<Vanta::UUID> {
        size_t operator()(const Vanta::UUID& uuid) const {
            return (uint64_t)uuid;
        }
    };
}
