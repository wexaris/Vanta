#pragma once

namespace Vanta {
    namespace Template {
        /// Remove references from a type.
        template<typename T>
        using RemoveRef = std::remove_reference<T>;

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
}