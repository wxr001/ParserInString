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

#ifndef PARSER_IN_STRING_COMMON_TAVLHELPER
#define PARSER_IN_STRING_COMMON_TAVLHELPER

#include "Common/Tstring.hpp"
#include "TAVL.hpp"
#include "Tlist.hpp"
#include "ValueWrapper.hpp"
namespace Compiler
{
    template <typename T>
    struct type_list_to_tavl_helper
    {
        using type = tavl::tavl_insert_t<
            typename type_list_to_tavl_helper<type_list_tail<T>>::type,
            type_list_top<T>>;
    };
    template <>
    struct type_list_to_tavl_helper<type_list<>>
    {
        using type = tavl::empty_node;
    };
    template <typename List>
    using type_list_to_tavl = typename type_list_to_tavl_helper<List>::type;
    template <typename List>
    struct type_list_to_tavl_merge_helper;
    template <typename T1, typename T2, typename... Trees>
    struct type_list_to_tavl_merge_helper<type_list<T1, T2, Trees...>>
    {
        static_assert(!(std::is_same_v<T1, tavl::Impl::invalid> ||
                        std::is_same_v<T2, tavl::Impl::invalid> ||
                        (std::is_same_v<Trees, tavl::Impl::invalid> || ...)),
                      "Invalid Tree");
        using type = tavl::tavl_union_t<T1, T2, Trees...>;
    };
    template <typename T1>
    struct type_list_to_tavl_merge_helper<type_list<T1>>
    {
        using type = T1;
    };
    template <typename TreeList>
    using type_list_to_tavl_merge =
        typename type_list_to_tavl_merge_helper<TreeList>::type;
    template <typename Old, typename New>
    struct tavl_merge_for_union
    {
        using type = typename std::conditional_t<
            tavl::tavl_is_same_v<Old, New>,
            identity<New>,
            lazy_template<tavl::tavl_union_t, Old, New>>::type;
    };
    template <typename T>
    struct is_tavl: std::false_type
    {};
    template <typename L, typename R, int H, typename K, typename V>
    struct is_tavl<tavl::tavl_node<L, R, H, K, V>>: std::true_type
    {};
    template <>
    struct is_tavl<tavl::empty_node>: std::true_type
    {};
    template <typename Key, typename Value>
    struct postpone_to_merge
    {
        using type = tavl::kv_pair<Key, Value>;
    };
    template <typename Key, typename>
    struct get_key
    {
        using type = Key;
    };
    template <typename T, typename K>
    using tavl_find_value = typename tavl::tavl_find_t<T, K>::value;
    template <typename T, typename K>
    using tavl_find_empty_node = std::conditional_t<tavl::tavl_contain_v<T, K>,
                                                    tavl_find_value<T, K>,
                                                    tavl::empty_node>;
    template <typename T1, typename T2, typename T3>
    using logical_or_merger = std::disjunction<T1, T2, T3>;
    template <typename T1, typename T2, typename T3>
    using logical_and_merger = std::conjunction<T1, T2, T3>;
} // namespace Compiler
namespace tavl
{
    template <char... lhs, char... rhs>
    struct compare<Compiler::t_string<lhs...>, Compiler::t_string<rhs...>>
    {
        using type = std::conditional_t<
            Compiler::first_character_v<lhs...> >=
                Compiler::first_character_v<rhs...>,
            std::conditional_t<
                (Compiler::first_character_v<lhs...> >
                 Compiler::first_character_v<rhs...>),
                std::integral_constant<int, 1>,
                typename compare<Compiler::ignore_one_t<lhs...>,
                                 Compiler::ignore_one_t<rhs...>>::type>,
            std::integral_constant<int, -1>>;
        static constexpr int value = type::value;
    };
    template <char... rhs>
    struct compare<Compiler::t_string<>, Compiler::t_string<rhs...>>
    {
        using type                 = std::integral_constant<int, -1>;
        static constexpr int value = type::value;
    };
    template <char... lhs>
    struct compare<Compiler::t_string<lhs...>, Compiler::t_string<>>
    {
        using type                 = std::integral_constant<int, 1>;
        static constexpr int value = type::value;
    };
    template <>
    struct compare<Compiler::t_string<>, Compiler::t_string<>>
    {
        using type                 = std::integral_constant<int, 0>;
        static constexpr int value = type::value;
    };
    template <auto L, auto R>
    struct compare<Compiler::value_wrapper<L>, Compiler::value_wrapper<R>>
    {
        static constexpr int value = L - R;
        using type                 = std::integral_constant<int, value>;
    };
} // namespace tavl
namespace tavl
{
    template <typename T, T v1, T v2>
    struct compare<std::integral_constant<T, v1>, std::integral_constant<T, v2>>
    {
        static constexpr int value = v1 - v2;
    };
    template <typename Arg11, typename Arg12, typename Arg21, typename Arg22>
    struct compare<Compiler::type_pair<Arg11, Arg12>,
                   Compiler::type_pair<Arg21, Arg22>>
    {
    private:
        static constexpr int comp_cur = compare_v<Arg11, Arg21>;
        using value_type              = typename std::conditional_t<
            !std::is_same_v<Arg11, Arg21>,
            identity<Compiler::value_wrapper<comp_cur>>,
            lazy_template<compare, Arg12, Arg22>>::type;

    public:
        static constexpr int value = value_type::value;
    };
    template <typename Arg1,
              typename... Args1,
              typename Arg2,
              typename... Args2>
    struct compare<Compiler::type_list<Arg1, Args1...>,
                   Compiler::type_list<Arg2, Args2...>>
    {
    private:
        static constexpr int comp_cur = compare_v<Arg1, Arg2>;
        using value_type              = typename std::conditional_t<
            !std::is_same_v<Arg1, Arg2>,
            identity<Compiler::value_wrapper<comp_cur>>,
            lazy_template<compare,
                          Compiler::type_list<Args1...>,
                          Compiler::type_list<Args2...>>>::type;

    public:
        static constexpr int value = value_type::value;
    };
    template <typename... Args2>
    struct compare<Compiler::type_list<>, Compiler::type_list<Args2...>>
    {
        static constexpr int value = -1;
    };
    template <typename... Args1>
    struct compare<Compiler::type_list<Args1...>, Compiler::type_list<>>
    {
        static constexpr int value = 1;
    };
    template <>
    struct compare<Compiler::type_list<>, Compiler::type_list<>>
    {
        static constexpr int value = 0;
    };
} // namespace tavl
#endif
