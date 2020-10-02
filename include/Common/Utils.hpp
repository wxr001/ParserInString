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

#ifndef PARSER_IN_STRING_COMMON_UTILS
#define PARSER_IN_STRING_COMMON_UTILS
#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include "static_print.hpp"
#include <type_traits>
namespace Compiler
{
    // empty symbol used for first-set when the original grammar has an empty
    // definition.
    using empty_symbol = type_list<t_string<'@', 'E', 'M', 'P', 'T', 'Y'>>;
    using empty_term   = type_pair<Impl::meta_identifier_flag,
                                 t_string<'@', 'E', 'M', 'P', 'T', 'Y'>>;
    using empty_set    = tavl::kv_pair<empty_symbol, std::true_type>;
    template <typename S, typename K, typename V = std::true_type>
    using set_try_insert_t = typename std::conditional_t<
        tavl::tavl_contain_v<S, K>,
        identity<S>,
        tavl::lazy_template<tavl::tavl_insert_t, S, K, V>>::type;
    template <typename S1, typename S2, unsigned K>
    struct set_addition
    {
    private:
        template <typename Elem>
        static constexpr bool check_length = type_list_size<Elem> >= K;
        template <typename Elem>
        static constexpr bool is_empty = std::is_same_v<Elem, empty_symbol>;

        using size_less = int;
        using size_more = long long;
        template <typename Left, typename Right, typename Cur>
        struct size_merger
        {
            static constexpr auto len_l = sizeof(Left);
            static constexpr auto len_r = sizeof(Right);
            static constexpr auto len_c = sizeof(Cur);
            using type                  = std::conditional_t<
                (len_l > len_r),
                std::conditional_t<(len_c > len_l), Cur, Left>,
                std::conditional_t<(len_c > len_r), Cur, Right>>;
        };
        template <typename Key, typename>
        struct check_S1_each_first_symbol_length
        {
            using type = std::conditional_t<check_length<Key> && !is_empty<Key>,
                                            size_less,
                                            size_more>;
        };
        static constexpr bool whether_S1_already_satisfy_requirement =
            std::is_same_v<
                tavl::tavl_for_each_t<S1,
                                      check_S1_each_first_symbol_length,
                                      size_merger,
                                      size_less>,
                size_less>;
        template <typename Key, typename>
        struct for_each_symbol_in_S1
        {
            template <typename Key2, typename>
            struct for_each_symbol_in_S2
            {
                using type =
                    type_list_first_n<type_list_push_back_m<Key, Key2>, K>;
            };
            template <typename V>
            struct for_each_symbol_in_S2<empty_symbol, V>
            {
                using type = Key;
            };
            template <typename T, typename C>
            struct merge_result
            {
                using type = set_try_insert_t<T, C>;
            };
            template <typename Set>
            struct lazy:
                identity<
                    tavl::tavl_for_each_middle_order_t<Set,
                                                       for_each_symbol_in_S2,
                                                       merge_result,
                                                       tavl::empty_node>>
            {};
            using type = typename std::conditional_t<
                check_length<Key>,
                lazy_template<identity,
                              tavl::tavl_insert_t<tavl::empty_node, Key>>,
                lazy_template<lazy, S2>>::type::type; // set
        };
        template <typename V>
        struct for_each_symbol_in_S1<empty_symbol, V>
        {
            using type = S2;
        };
        template <typename S>
        using addition = tavl::tavl_for_each_t<S,
                                               for_each_symbol_in_S1,
                                               tavl::tavl_union_3,
                                               tavl::empty_node>;

    public:
        using type = typename std::conditional_t<
            whether_S1_already_satisfy_requirement,
            identity<S1>,
            // S (+) empty should be empty if S doesn't satisfy the
            // condition(length of k)
            std::conditional_t<std::is_same_v<S2, tavl::empty_node>,
                               identity<tavl::empty_node>,
                               lazy_template<addition, S1>>>::type;
    };
    template <typename S1, typename S2, unsigned K>
    using set_addition_t = typename set_addition<S1, S2, K>::type;
    template <typename Definition>
    struct has_terminals_only;
    template <typename... Terms>
    struct has_terminals_only<type_list<Terms...>>
    {
        static constexpr bool value =
            ((std::is_same_v<typename Terms::first_type,
                             Impl::terminal_string_flag> ||
              std::is_same_v<typename Terms::first_type,
                             Impl::empty_sequence_flag>)&&...);
    };
    template <typename T>
    static constexpr bool has_terminals_only_v = has_terminals_only<T>::value;
    template <typename L>
    struct remove_empty_from_list;
    template <typename Item, typename... Others>
    struct remove_empty_from_list<type_list<Item, Others...>>
    {
        using type = type_list_push_front<
            typename remove_empty_from_list<type_list<Others...>>::type,
            Item>;
    };
    template <typename... Others>
    struct remove_empty_from_list<type_list<type_list<>, Others...>>
    {
        using type =
            typename remove_empty_from_list<type_list<Others...>>::type;
    };
    template <>
    struct remove_empty_from_list<type_list<>>
    {
        using type = type_list<>;
    };
    template <typename T>
    using remove_empty_from_list_t = typename remove_empty_from_list<T>::type;
    template <typename Rules,
              template <typename Target, typename RemainingDef, typename Symbol>
              typename SymbolFunctor,
              template <typename L, typename R, typename C>
              typename RuleMerger,
              template <typename Last, typename Curr>
              typename DefinitionMerger,
              template <typename Last, typename Curr>
              typename SymbolMerger,
              typename DefaultRule,
              typename DefaultDefinition,
              typename DefaultSymbol,
              typename Targets = tavl::empty_node>
    struct for_each_symbol
    {
    private:
        template <typename Target, typename SymList, typename LastSymResult>
        struct for_each_symbol_impl
        {
            using result = typename SymbolFunctor<Target,
                                                  SymList,
                                                  type_list_top<SymList>>::type;
            using type   = typename for_each_symbol_impl<
                Target,
                type_list_tail<SymList>,
                typename SymbolMerger<LastSymResult, result>::type>::type;
        };
        template <typename Target, typename Result>
        struct for_each_symbol_impl<Target, type_list<>, Result>
        {
            using type = Result;
        };
        template <typename Target, typename DefList, typename LastDefResult>
        struct for_each_definition
        {
            using current = typename DefinitionMerger<
                LastDefResult,
                typename for_each_symbol_impl<Target,
                                              type_list_top<DefList>,
                                              DefaultSymbol>::type>::type;
            using type = typename for_each_definition<Target,
                                                      type_list_tail<DefList>,
                                                      current>::type;
        };
        template <typename Target, typename Result>
        struct for_each_definition<Target, type_list<>, Result>
        {
            using type = Result;
        };
        template <typename Target, typename Defs>
        struct for_each_rule
        {
            using type =
                tavl::kv_pair<Target,
                              typename std::conditional_t<
                                  std::is_same_v<Targets, tavl::empty_node> ||
                                      tavl::tavl_contain_v<Targets, Target>,
                                  lazy_template<for_each_definition,
                                                Target,
                                                Defs,
                                                DefaultDefinition>,
                                  identity<identity<DefaultRule>>>::type::type>;
        };

    public:
        using type = tavl::
            tavl_for_each_t<Rules, for_each_rule, RuleMerger, DefaultRule>;
    };
    template <typename Rules,
              template <typename Target, typename RemainingDef, typename Symbol>
              typename SymbolFunctor,
              template <typename L, typename R, typename C>
              typename RuleMerger,
              template <typename Last, typename Curr>
              typename DefinitionMerger,
              template <typename Last, typename Curr>
              typename SymbolMerger,
              typename DefaultRule,
              typename DefaultDefinition,
              typename DefaultSymbol,
              typename Targets = tavl::empty_node>
    using for_each_symbol_t = typename for_each_symbol<Rules,
                                                       SymbolFunctor,
                                                       RuleMerger,
                                                       DefinitionMerger,
                                                       SymbolMerger,
                                                       DefaultRule,
                                                       DefaultDefinition,
                                                       DefaultSymbol,
                                                       Targets>::type;

    template <unsigned Value>
    struct integer_generator
    {
        using type                  = value_wrapper<Value>;
        static constexpr auto value = Value;
        using next                  = integer_generator<Value + 1>;
    };
} // namespace Compiler
namespace tavl
{
#define COMP_DEF(flag1, flag2)                                           \
    template <>                                                          \
    struct compare<Compiler::Impl::flag1, Compiler::Impl::flag2>         \
    {                                                                    \
        static constexpr int value =                                     \
            Compiler::Impl::flag1::value - Compiler::Impl::flag2::value; \
    }
#define COMP_DEFS(flag)                     \
    COMP_DEF(flag, empty_sequence_flag);    \
    COMP_DEF(flag, grouped_sequence_flag);  \
    COMP_DEF(flag, meta_identifier_flag);   \
    COMP_DEF(flag, optional_sequence_flag); \
    COMP_DEF(flag, repeated_sequence_flag); \
    COMP_DEF(flag, special_sequence_flag);  \
    COMP_DEF(flag, terminal_string_flag)
    COMP_DEFS(empty_sequence_flag);
    COMP_DEFS(grouped_sequence_flag);
    COMP_DEFS(meta_identifier_flag);
    COMP_DEFS(optional_sequence_flag);
    COMP_DEFS(repeated_sequence_flag);
    COMP_DEFS(special_sequence_flag);
    COMP_DEFS(terminal_string_flag);
#undef COMP_DEFS
#undef COMP_DEF
} // namespace tavl
#endif
