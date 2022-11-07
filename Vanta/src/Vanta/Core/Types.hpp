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


    /// ///////////////// OPTIONALS //////////////////

    template<typename T>
    using Opt = std::optional<T>;

    constexpr std::nullopt_t None = std::nullopt;


    /// ///////////////// TEMPLATES //////////////////

    /// Remove references from a type.
    template<typename T>
    using RemoveRef   = std::remove_reference<T>;

    template<typename T>
    using RemoveRef_t = typename RemoveRef<T>::type;

    /// Check if one type is the base of another.
    template<typename Base, typename Derived>
    using IsBase = std::is_base_of<Base, Derived>;

    template<typename Base, typename Derived>
    constexpr bool IsBase_v = IsBase<Base, Derived>::value;

    /// Get N-th type in a typelist.
    template<usize N, typename... Types>
    struct Get : std::tuple_element<N, std::tuple<Types...>> {};

    template<usize N, typename... Types>
    using Get_t = typename Get<N, Types...>::type;

    /// Check if a typelist contains a certain type.
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

    /// Find a type given a certain condition.
    template<template<typename> typename Cond, typename... Types>
    struct Find;

    template<template<typename> typename Cond>
    struct Find<Cond> { using type = void; };

    template<template<typename> typename Cond, typename Head, typename... Tail>
    struct Find<Cond, Head, Tail...> {
        using type = typename std::conditional_t<Cond<Head>::value, Head, typename Find<Cond, Tail...>::type>;
    };

    template<template<typename> typename Cond, typename... Types>
    using Find_t = Find<Cond, Types...>::type;
}

#include "Vanta/Util/Time.hpp"
