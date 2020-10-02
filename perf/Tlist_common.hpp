// ParserInString -*- C++ -*-

// Copyright (C) 2019 Scott Brown

// This file is part of the UCL library.
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PARSER_IN_STRING_COMMON_TLIST
#define PARSER_IN_STRING_COMMON_TLIST

#include "../include/Common/Identity.hpp"
#include <type_traits>

namespace Compiler
{
    template <typename T1, typename T2>
    struct type_pair
    {
        using first_type  = T1;
        using second_type = T2;
    };
    template <typename... elem>
    struct type_list
    {};
    template <typename T>
    struct is_type_list: std::false_type
    {};
    template <typename... elem>
    struct is_type_list<type_list<elem...>>: std::true_type
    {};
    template <typename T>
    inline constexpr bool is_type_list_v = is_type_list<T>::value;
    template <typename T, int idx = 0>
    struct type_list_helper;
    template <typename elem, typename... tails, int idx>
    struct type_list_helper<type_list<elem, tails...>, idx>: identity<elem>
    {
        static constexpr int  index = idx;
        static constexpr auto size  = 1 + sizeof...(tails);
        using tail                  = type_list<tails...>;
        template <typename T>
        using push_back = type_list<elem, tails..., T>;
        template <typename T>
        using push_front = type_list<T, elem, tails...>;
        template <typename T, int n>
        struct push_back_n_helper
        {
            using type = typename type_list_helper<
                typename push_back_n_helper<T, n - 1>::type>::
                template push_back<T>;
        };
        template <typename T>
        struct push_back_n_helper<T, 0>: identity<type_list<elem, tails...>>
        {};
        template <typename T, int n>
        using push_back_n = typename push_back_n_helper<T, n>::type;
        template <typename T>
        struct push_back_m_helper: identity<push_back<T>>
        {};
        template <typename... elems>
        struct push_back_m_helper<type_list<elems...>>:
            identity<type_list<elem, tails..., elems...>>
        {};
        template <typename T>
        using push_back_m = typename push_back_m_helper<T>::type;
        template <typename T>
        struct push_front_m_helper: identity<push_front<T>>
        {};
        template <typename... elems>
        struct push_front_m_helper<type_list<elems...>>:
            identity<type_list<elems..., elem, tails...>>
        {};
        template <typename T>
        using push_front_m = typename push_front_m_helper<T>::type;
        template <typename T>
        static constexpr int
            find = (std::is_same_v<elem, T> ?
                        index :
                        type_list_helper<tail, index + 1>::template find<T>);
        template <int n>
        struct lazy_get_false:
            identity<
                typename type_list_helper<tail, index + 1>::template get<n>>
        {};
        template <int n>
        using get = typename std::
            conditional_t<index == n, identity<elem>, lazy_get_false<n>>::type;
    };
    template <int idx>
    struct type_list_helper<type_list<>, idx>
    {
        static constexpr int  index = -1;
        static constexpr auto size  = 0;
        using tail                  = type_list<>;
        template <typename T>
        using push_back = type_list<T>;
        template <typename T>
        using push_front = type_list<T>;
        template <typename T, int n>
        struct push_back_n_helper
        {
            using type = typename type_list_helper<
                typename push_back_n_helper<T, n - 1>::type>::
                template push_back<T>;
        };
        template <typename T>
        struct push_back_n_helper<T, 0>: identity<type_list<>>
        {};
        template <typename T, int n>
        using push_back_n = typename push_back_n_helper<T, n>::type;
        template <typename T>
        struct push_back_m_helper: identity<push_back<T>>
        {};
        template <typename... elems>
        struct push_back_m_helper<type_list<elems...>>:
            identity<type_list<elems...>>
        {};
        template <typename T>
        using push_back_m = typename push_back_m_helper<T>::type;
        template <typename T>
        struct push_front_m_helper: identity<push_front<T>>
        {};
        template <typename... elems>
        struct push_front_m_helper<type_list<elems...>>:
            identity<type_list<elems...>>
        {};
        template <typename T>
        using push_front_m = typename push_front_m_helper<T>::type;
        template <template <typename> typename Apply>
        struct for_each_helper
        {
            using type = type_list<>;
        };

        template <typename T>
        static constexpr int find = -1;
    };
    template <typename T>
    inline constexpr auto type_list_size = type_list_helper<T>::size;
    template <typename T>
    using type_list_top = typename type_list_helper<T>::type;
    template <typename T>
    using type_list_tail = typename type_list_helper<T>::tail;
    template <typename T, typename val>
    using type_list_push_back =
        typename type_list_helper<T>::template push_back<val>;
    template <typename T, typename val>
    using type_list_push_front =
        typename type_list_helper<T>::template push_front<val>;
    template <typename T, typename val, int n>
    using type_list_push_back_n =
        typename type_list_helper<T>::template push_back_n<val, n>;
    template <typename T, typename val>
    using type_list_push_back_m =
        typename type_list_helper<T>::template push_back_m<
            val>; //! merge val into T if val is type_list, push_back otherwise

    template <typename T, template <typename> typename Apply>
    struct type_list_for_each_helper;
    template <typename elem,
              template <typename>
              typename Apply,
              typename... tails>
    struct type_list_for_each_helper<type_list<elem, tails...>, Apply>
    {
        using type = type_list_push_front<
            typename type_list_for_each_helper<type_list<tails...>,
                                               Apply>::type,
            typename Apply<elem>::type>;
    };
    template <template <typename> typename Apply>
    struct type_list_for_each_helper<type_list<>, Apply>
    {
        using type = type_list<>;
    };
    template <typename T, template <typename> typename Apply>
    using type_list_for_each =
        typename type_list_for_each_helper<T, Apply>::type;

    template <typename T, typename val, int idx = 0>
    struct type_list_find_helper
    {
        static constexpr int value =
            (std::is_same_v<type_list_top<T>, val> ?
                 idx :
                 type_list_find_helper<type_list_tail<T>, val, idx + 1>::value);
    };
    template <typename val, int idx>
    struct type_list_find_helper<type_list<>, val, idx>
    {
        static constexpr int value = -1; // not fount
    };
    template <typename T, typename val>
    inline constexpr int type_list_find =
        type_list_helper<T>::template find<val>;

    template <typename T, int idx>
    using type_list_get = typename type_list_helper<T>::template get<idx>;

    template <typename Result, typename T, typename K>
    struct type_list_first_n_helper;
    template <typename... Results, typename... Elems, typename K>
    struct type_list_first_n_helper<type_list<Results...>,
                                    type_list<Elems...>,
                                    K>
    {
        static_assert(sizeof...(Results) <= K::value, "Result type overflows!");
        using type = typename std::conditional_t<
            sizeof...(Results) == K::value,
            lazy_template<identity, type_list<Results...>>,
            lazy_template<
                type_list_first_n_helper,
                type_list<Results..., type_list_top<type_list<Elems...>>>,
                type_list_tail<type_list<Elems...>>,
                K>>::type::type;
    };
    template <typename... Results, typename K>
    struct type_list_first_n_helper<type_list<Results...>, type_list<>, K>
    {
        static_assert(sizeof...(Results) <= K::value, "Result type overflows!");
        using type = type_list<Results...>;
    };
    template <typename T, int n>
    using type_list_first_n =
        typename type_list_first_n_helper<type_list<>,
                                          T,
                                          std::integral_constant<int, n>>::type;
    template <typename T, typename Val>
    using type_list_try_push_front =
        std::conditional_t<type_list_find<T, Val> == -1,
                           type_list_push_front<T, Val>,
                           T>;
    template <typename T, typename L>
    using type_list_push_front_m =
        typename type_list_helper<T>::template push_front_m<L>;
    // tlist-set
    template <typename S, typename... Vs>
    struct tlist_set_try_insert_impl;
    template <typename S, typename V, typename... Vs>
    struct tlist_set_try_insert_impl<S, V, Vs...>
    {
        using type =
            typename tlist_set_try_insert_impl<type_list_push_front<S, V>,
                                               Vs...>::type;
    };
    template <typename S>
    struct tlist_set_try_insert_impl<S>
    {
        using type = S;
    };
    template <typename S, typename... V>
    using tlist_set_try_insert =
        typename tlist_set_try_insert_impl<S, V...>::type;
    template <typename S, typename S2>
    struct tlist_set_union_impl;
    template <typename S, typename... Vs>
    struct tlist_set_union_impl<S, type_list<Vs...>>
    {
        using type = tlist_set_try_insert<S, Vs...>;
    };
    template <typename S, typename S2>
    using tlist_set_union = typename tlist_set_union_impl<S, S2>::type;
    template <typename S1, typename S2, typename S3>
    using tlist_set_union_3 =
        identity<tlist_set_union<tlist_set_union<S1, S2>, S3>>;
    template <typename S, typename S2>
    struct tlist_set_not_in_s_impl
    {
        using current = type_list_top<S2>;
        using type    = typename std::conditional_t<
            type_list_find<S, current> == -1,
            lazy_template<
                type_list_push_front,
                typename tlist_set_not_in_s_impl<S, type_list_tail<S2>>::type,
                current>,
            tlist_set_not_in_s_impl<S, type_list_tail<S2>>>::type;
    };
    template <typename S>
    struct tlist_set_not_in_s_impl<S, type_list<>>
    {
        using type = type_list<>;
    };
    template <typename S,
              template <typename T>
              typename F,
              template <typename Before, typename Current>
              typename M,
              typename Default>
    struct tlist_set_for_each_impl
    {
        using type = typename M<
            typename tlist_set_for_each_impl<type_list_tail<S>, F, M, Default>::
                type,
            typename F<type_list_top<S>>::type>::type;
    };
    template <template <typename T> typename F,
              template <typename Before, typename Current>
              typename M,
              typename Default>
    struct tlist_set_for_each_impl<type_list<>, F, M, Default>
    {
        using type = Default;
    };
    template <typename S,
              template <typename T>
              typename F,
              template <typename Before, typename Current>
              typename M,
              typename Default>
    using tlist_set_for_each =
        typename tlist_set_for_each_impl<S, F, M, Default>::type;
    template <typename S, typename S2>
    using tlist_set_not_in_s = typename tlist_set_not_in_s_impl<S, S2>::type;
    namespace InHeaderDebug
    {
        using simple_list = type_list<int, float, double>;
        using inner_list  = type_list<type_list<int, float, double>, char>;
        using empty_list  = type_list<>;
        inline namespace TestSize
        {
            static_assert(type_list_size<simple_list> == 3, "size check");
            static_assert(type_list_size<empty_list> == 0, "empty size check");
            static_assert(type_list_size<inner_list> == 2,
                          "list with inner list");
        } // namespace TestSize
        inline namespace TestTop
        {
            static_assert(std::is_same_v<type_list_top<simple_list>, int>,
                          "normal top check");
            static_assert(
                std::is_same_v<type_list_top<inner_list>, simple_list>,
                "inner top check");
            static_assert(
                std::is_same_v<type_list_top<type_list_top<inner_list>>, int>,
                "mixed top check");
            // static_assert(std::is_same_v<type_list_top<empty_list>, int>,
            //               "empty top check");
        } // namespace TestTop
        inline namespace TestTail
        {
            static_assert(std::is_same_v<type_list_tail<simple_list>,
                                         type_list<float, double>>,
                          "tail check");
            static_assert(
                std::is_same_v<type_list_tail<inner_list>, type_list<char>>,
                "inner list check && single element in tail");
            static_assert(
                std::is_same_v<type_list_tail<empty_list>, type_list<>>,
                "empty check");
            static_assert(
                std::is_same_v<type_list_tail<type_list_tail<inner_list>>,
                               type_list<>>,
                "cddr check");
            static_assert(
                std::is_same_v<type_list_tail<type_list_top<inner_list>>,
                               type_list<float, double>>,
                "cadr check");
        } // namespace TestTail
        inline namespace TestPushBack
        {
            static_assert(std::is_same_v<type_list_push_back<simple_list, char>,
                                         type_list<int, float, double, char>>,
                          "normal push_back");
            static_assert(
                std::is_same_v<type_list_push_back<simple_list, simple_list>,
                               type_list<int, float, double, simple_list>>,
                "push_back instead of merging");
            static_assert(
                std::is_same_v<type_list_push_back<empty_list, simple_list>,
                               type_list<simple_list>>,
                "empty push_back");
            static_assert(
                std::is_same_v<type_list_push_back<
                                   type_list_push_back<empty_list, simple_list>,
                                   char>,
                               inner_list>,
                "multiple push_back");
        } // namespace TestPushBack
        inline namespace TestPushFront
        {
            static_assert(
                std::is_same_v<type_list_push_front<simple_list, char>,
                               type_list<char, int, float, double>>,
                "normal push_front");
            static_assert(
                std::is_same_v<type_list_push_front<simple_list, simple_list>,
                               type_list<simple_list, int, float, double>>,
                "push_front instead of merging");
            static_assert(
                std::is_same_v<type_list_push_front<empty_list, simple_list>,
                               type_list<simple_list>>,
                "empty push_front");
            static_assert(
                std::is_same_v<
                    type_list_push_front<type_list_push_front<empty_list, char>,
                                         simple_list>,
                    inner_list>,
                "multiple push_front");
        } // namespace TestPushFront
        inline namespace TestPushBackN
        {
            static_assert(
                std::is_same_v<type_list_push_back_n<simple_list, char, 1>,
                               type_list<int, float, double, char>>,
                "normal push_back_1");
            static_assert(
                std::is_same_v<
                    type_list_push_back_n<simple_list, simple_list, 2>,
                    type_list<int, float, double, simple_list, simple_list>>,
                "push_back_2 instead of merging");
            static_assert(std::is_same_v<
                              type_list_push_back_n<empty_list, simple_list, 2>,
                              type_list<simple_list, simple_list>>,
                          "empty push_back_2");
            static_assert(std::is_same_v<
                              type_list_push_back_n<empty_list, simple_list, 0>,
                              empty_list>,
                          "push_back_0");
        } // namespace TestPushBackN
        inline namespace TestPushBackM
        {
            static_assert(
                std::is_same_v<type_list_push_back_m<empty_list, simple_list>,
                               simple_list>,
                "empty merge");
            static_assert(
                std::is_same_v<
                    type_list_push_back_m<simple_list, simple_list>,
                    type_list<int, float, double, int, float, double>>,
                "multiple elements merge");
            static_assert(
                std::is_same_v<type_list_push_back_m<empty_list, empty_list>,
                               empty_list>,
                "empty-empty merge");
            static_assert(
                std::is_same_v<type_list_push_back_m<empty_list, char>,
                               type_list<char>>,
                "empty no merging");
            static_assert(
                std::is_same_v<type_list_push_back_m<simple_list, char>,
                               type_list<int, float, double, char>>,
                "multiple elements no merging");
        } // namespace TestPushBackM
        inline namespace TestForEach
        {
            template <typename T>
            struct for_each_test
            {
                using type = std::integral_constant<std::size_t, sizeof(T)>;
            };
            static_assert(
                std::is_same_v<type_list_for_each<simple_list, for_each_test>,
                               type_list<typename for_each_test<int>::type,
                                         typename for_each_test<float>::type,
                                         typename for_each_test<double>::type>>,
                "normal for_each");
            static_assert(
                std::is_same_v<type_list_for_each<empty_list, for_each_test>,
                               empty_list>,
                "empty for_each");
        } // namespace TestForEach

    } // namespace InHeaderDebug
} // namespace Compiler

#endif
