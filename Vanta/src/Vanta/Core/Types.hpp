#pragma once
#include <filesystem>
#include <memory>

namespace Vanta {

    /// ///////////////// NUMBERS ////////////////////

    using uint = unsigned int;

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

    template<typename T>
    using RemoveRef   = std::remove_reference<T>;

    template<typename T>
    using RemoveRef_t = typename RemoveRef<T>::type;


    template<typename Base, typename Derived>
    using IsBase = std::is_base_of<Base, Derived>;

    template<typename Base, typename Derived>
    constexpr bool IsBase_v = IsBase<Base, Derived>::value;


    template<usize N, typename... Types>
    struct Get : std::tuple_element<N, std::tuple<Types...>> {};

    template<usize N, typename... Types>
    using Get_t = typename Get<N, Types...>::type;


    template <typename T, typename... Types>
    struct Contains;

    template <typename T>
    struct Contains<T> : std::false_type {};

    template <typename T, typename... Types>
    struct Contains<T, T, Types...> : std::true_type {};

    template <typename T, typename U, typename... Types>
    struct Contains<T, U, Types...> : Contains<T, Types...> {};

    template <typename T, typename... Types>
    constexpr bool Contains_v = Contains<T, Types...>::value;
}

#include "Vanta/Util/Time.hpp"
