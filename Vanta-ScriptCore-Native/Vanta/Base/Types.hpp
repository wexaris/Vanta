#pragma once
#include <filesystem>
#include <memory>

namespace Vanta {

    /// ///////////////// NUMBERS ////////////////////

    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;
    using isize = ptrdiff_t;

    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;
    using usize = size_t;

    using uint = uint32;


    /// ///////////////// PATH ///////////////////////

    /// <summary>
    /// Filesystem path type.
    /// </summary>
    using Path = std::filesystem::path;


    /// ///////////////// MEMORY /////////////////////

    /// <summary>
    /// Exclusive smart pointer.
    /// </summary>
    template<typename T>
    using Box = std::unique_ptr<T>;

    /// <summary>
    /// Shared smart pointer.
    /// </summary>
    template<typename T>
    using Ref = std::shared_ptr<T>;

    /// <summary>
    /// Creates a new exclusive smart pointer.
    /// Arguments get passed to the underlying type's constructor.
    /// </summary>
    /// <param name="args">Constructor arguments</param>
    template<typename T, typename... Args>
    constexpr Box<T> NewBox(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    /// <summary>
    /// Creates a new shared smart pointer.
    /// Arguments get passed to the underlying type's constructor.
    /// </summary>
    /// <param name="args">Constructor arguments</param>
    template<typename T, typename... Args>
    constexpr Ref<T> NewRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    /// <summary>
    /// Casts a shared smart pointer to a different type.
    /// </summary>
    /// <param name="from">Pointer to be cast to a different type</param>
    template<class To, class From>
    Ref<To> RefCast(const Ref<From>& from) {
        return std::static_pointer_cast<To>(from);
    }

    /// <summary>
    /// Casts a shared smart pointer to a different type.
    /// </summary>
    /// <param name="from">Pointer to be cast to a different type</param>
    template<class To, class From>
    Ref<To> RefCast(Ref<From>&& from) {
        return std::static_pointer_cast<To>(from);
    }


    /// ///////////////// OPTIONALS //////////////////

    template<typename T>
    using Opt = std::optional<T>;

    constexpr std::nullopt_t None = std::nullopt;
}
