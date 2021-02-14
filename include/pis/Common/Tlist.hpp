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

#include "Identity.hpp"
#include <type_traits>

namespace pis
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
    template <typename L>
    struct type_list_size_helper;
    template <typename... Elems>
    struct type_list_size_helper<type_list<Elems...>>
    {
        static constexpr auto value = sizeof...(Elems);
    };
    template <typename L>
    inline constexpr auto type_list_size = type_list_size_helper<L>::value;
    template <typename T>
    struct type_list_top_helper;
    template <typename E, typename... Es>
    struct type_list_top_helper<type_list<E, Es...>>
    {
        using type = E;
    };
    template <typename T>
    using type_list_top = typename type_list_top_helper<T>::type;

    template <typename T>
    struct type_list_tail_helper;
    template <typename E, typename... Es>
    struct type_list_tail_helper<type_list<E, Es...>>
    {
        using type = type_list<Es...>;
    };
    template <>
    struct type_list_tail_helper<type_list<>>
    {
        using type = type_list<>;
    };
    template <typename T>
    using type_list_tail = typename type_list_tail_helper<T>::type;
    template <typename, typename>
    struct type_list_push_back_helper;
    template <typename... Elems, typename T>
    struct type_list_push_back_helper<type_list<Elems...>, T>
    {
        using type = type_list<Elems..., T>;
    };
    template <typename L, typename T>
    using type_list_push_back = typename type_list_push_back_helper<L, T>::type;
    template <typename, typename>
    struct type_list_push_front_helper;
    template <typename... Elems, typename T>
    struct type_list_push_front_helper<type_list<Elems...>, T>
    {
        using type = type_list<T, Elems...>;
    };
    template <typename L, typename T>
    using type_list_push_front =
        typename type_list_push_front_helper<L, T>::type;
    template <typename L, typename V, int N>
    struct type_list_push_back_n_helper:
        type_list_push_back_n_helper<type_list_push_back<L, V>, V, N - 1>
    {};
    template <typename L, typename V>
    struct type_list_push_back_n_helper<L, V, 0>
    {
        using type = L;
    };
    template <typename L, typename V, int N>
    using type_list_push_back_n =
        typename type_list_push_back_n_helper<L, V, N>::type;
    template <typename L, typename V>
    struct type_list_push_back_m_helper;
    template <typename... Elems, typename V>
    struct type_list_push_back_m_helper<type_list<Elems...>, V>
    {
        using type = type_list<Elems..., V>;
    };
    template <typename... Elems, typename... Vs>
    struct type_list_push_back_m_helper<type_list<Elems...>, type_list<Vs...>>
    {
        using type = type_list<Elems..., Vs...>;
    };
    template <typename L, typename T>
    using type_list_push_back_m = typename type_list_push_back_m_helper<L, T>::
        type; //! merge T into L if val is type_list, push_back otherwise

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
    struct type_list_find_helper;
    template <typename... Elems, typename V, int idx>
    struct type_list_find_helper<type_list<V, Elems...>, V, idx>
    {
        static constexpr int value = idx;
    };
    template <typename E, typename... Elems, typename V, int idx>
    struct type_list_find_helper<type_list<E, Elems...>, V, idx>:
        type_list_find_helper<type_list<Elems...>, V, idx + 1>
    {};
    template <typename val, int idx>
    struct type_list_find_helper<type_list<>, val, idx>
    {
        static constexpr int value = -1; // not fount
    };
    template <typename T, typename val>
    inline constexpr int type_list_find = type_list_find_helper<T, val>::value;

    template <typename T, int N>
    struct type_list_get_helper;
    template <typename E, typename... Elems, int N>
    struct type_list_get_helper<type_list<E, Elems...>, N>:
        type_list_get_helper<type_list<Elems...>, N - 1>
    {};
    template <typename E, typename... Elems>
    struct type_list_get_helper<type_list<E, Elems...>, 0>
    {
        using type = E;
    };
    template <int N>
    struct type_list_get_helper<type_list<>, N>
    {};
    template <>
    struct type_list_get_helper<type_list<>, 0>
    {};
    template <typename L, int N>
    using type_list_get = typename type_list_get_helper<L, N>::type;

    template <typename Result, typename T, int N>
    struct type_list_first_n_helper;
    template <typename... Results, typename E, typename... Elems, int N>
    struct type_list_first_n_helper<type_list<Results...>,
                                    type_list<E, Elems...>,
                                    N>
    {
        using type = typename type_list_first_n_helper<type_list<Results..., E>,
                                                       type_list<Elems...>,
                                                       N - 1>::type;
    };
    template <typename... R, typename E, typename... Elems>
    struct type_list_first_n_helper<type_list<R...>, type_list<E, Elems...>, 0>
    {
        using type = type_list<R...>;
    };
    template <typename... R, int N>
    struct type_list_first_n_helper<type_list<R...>, type_list<>, N>
    {
        using type = type_list<R...>;
    };
    template <typename... R>
    struct type_list_first_n_helper<type_list<R...>, type_list<>, 0>
    {
        using type = type_list<R...>;
    };
    template <typename T, int n>
    using type_list_first_n =
        typename type_list_first_n_helper<type_list<>, T, n>::type;
    template <typename T, typename Val>
    using type_list_try_push_front =
        std::conditional_t<type_list_find<T, Val> == -1,
                           type_list_push_front<T, Val>,
                           T>;
    template <typename L, typename V>
    struct type_list_push_front_m_helper;
    template <typename... Elems, typename V>
    struct type_list_push_front_m_helper<type_list<Elems...>, V>
    {
        using type = type_list<V, Elems...>;
    };
    template <typename... Elems, typename... Vs>
    struct type_list_push_front_m_helper<type_list<Elems...>, type_list<Vs...>>
    {
        using type = type_list<Vs..., Elems...>;
    };
    template <typename L, typename T>
    using type_list_push_front_m =
        typename type_list_push_front_m_helper<L, T>::type;
    template <typename L, typename V>
    struct type_list_remove_helper;
    template <typename T, typename... Ts, typename V>
    struct type_list_remove_helper<type_list<T, Ts...>, V>
    {
        using type = type_list_push_front<
            typename type_list_remove_helper<type_list<Ts...>, V>::type,
            T>;
    };
    template <typename... Ts, typename V>
    struct type_list_remove_helper<type_list<V, Ts...>, V>
    {
        using type =
            typename type_list_remove_helper<type_list<Ts...>, V>::type;
    };
    template <typename V>
    struct type_list_remove_helper<type_list<>, V>
    {
        using type = type_list<>;
    };
    template <typename L, typename T>
    using type_list_remove = typename type_list_remove_helper<L, T>::type;
    // tlist-set
    template <typename S, typename... Vs>
    struct tlist_set_try_insert_impl;
    template <typename S, typename V, typename... Vs>
    struct tlist_set_try_insert_impl<S, V, Vs...>
    {
        using type =
            typename tlist_set_try_insert_impl<type_list_try_push_front<S, V>,
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
    template <typename S, typename V>
    using tlist_set_try_insert_2 = tlist_set_try_insert_impl<S, V>;
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
    template <typename S, typename S2>
    using tlist_set_not_in_s = typename tlist_set_not_in_s_impl<S, S2>::type;
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

    namespace in_header_debug
    {
        using simple_list = type_list<int, float, double>;
        using inner_list  = type_list<type_list<int, float, double>, char>;
        using empty_list  = type_list<>;
        inline namespace test_size
        {
            static_assert(type_list_size<simple_list> == 3, "size check");
            static_assert(type_list_size<empty_list> == 0, "empty size check");
            static_assert(type_list_size<inner_list> == 2,
                          "list with inner list");
        } // namespace test_size
        inline namespace test_top
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
        } // namespace test_top
        inline namespace test_tail
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
        } // namespace test_tail
        inline namespace test_push_back
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
        } // namespace test_push_back
        inline namespace test_push_front
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
        } // namespace test_push_front
        inline namespace test_push_back_n
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
        } // namespace test_push_back_n
        inline namespace test_push_back_m
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
        } // namespace test_push_back_m
        inline namespace test_for_each
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
        } // namespace test_for_each
        inline namespace test_find
        {
            static_assert(type_list_find<simple_list, int> == 0, "front find");
            static_assert(type_list_find<simple_list, double> == 2,
                          "back find");
            static_assert(type_list_find<simple_list, long> == -1,
                          "not-exist find");
            static_assert(type_list_find<empty_list, int> == -1, "empty find");
        } // namespace test_find
        inline namespace test_get
        {
            static_assert(std::is_same_v<type_list_get<simple_list, 0>, int>,
                          "front get");
            static_assert(std::is_same_v<type_list_get<simple_list, 2>, double>,
                          "back get");
            // static_assert(std::is_same_v<type_list_get<simple_list, 3>, int>,
            // "OOR");
            // static_assert(std::is_same_v<type_list_get<empty_list, 0>, int>,
            // "OOR-empty");
        } // namespace test_get
        inline namespace test_first_n
        {
            static_assert(
                std::is_same_v<type_list_first_n<simple_list, 0>, empty_list>,
                "min first_n");
            static_assert(
                std::is_same_v<type_list_first_n<simple_list, 3>, simple_list>,
                "max first_n");
            static_assert(
                std::is_same_v<type_list_first_n<empty_list, 0>, empty_list>,
                "empty first_n");
        } // namespace test_first_n
        inline namespace test_try_push_front
        {
            static_assert(
                std::is_same_v<type_list_try_push_front<simple_list, int>,
                               simple_list>,
                "no push_front");
            static_assert(
                std::is_same_v<type_list_try_push_front<simple_list, long>,
                               type_list_push_front<simple_list, long>>,
                "do push_front");
        } // namespace test_try_push_front
        inline namespace test_push_front_m
        {
            static_assert(
                std::is_same_v<type_list_push_front_m<empty_list, simple_list>,
                               simple_list>,
                "empty merge");
            static_assert(
                std::is_same_v<
                    type_list_push_front_m<simple_list,
                                           type_list_tail<simple_list>>,
                    type_list<float, double, int, float, double>>,
                "multiple elements merge");
            static_assert(
                std::is_same_v<type_list_push_front_m<empty_list, empty_list>,
                               empty_list>,
                "empty-empty merge");
            static_assert(
                std::is_same_v<type_list_push_front_m<empty_list, char>,
                               type_list<char>>,
                "empty no merging");
            static_assert(
                std::is_same_v<type_list_push_front_m<simple_list, char>,
                               type_list<char, int, float, double>>,
                "multiple elements no merging");
        } // namespace test_push_front_m

    } // namespace in_header_debug
} // namespace pis

#endif
