#pragma once

namespace Vanta {

    template<typename T>
    class Singleton {
    public:
        static T& Get() {
            static T instance;
            return instance;
        }

        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton&&) = delete;

    protected:
        Singleton() = default;
        virtual ~Singleton() = default;
    };
}
