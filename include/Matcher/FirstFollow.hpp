// ParserInString -*- C++ -*-

// Copyright (C) 2019-2020 Scott Brown

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

#ifndef PARSER_IN_STRING_MATCHER_FIRSTFOLLOW
#define PARSER_IN_STRING_MATCHER_FIRSTFOLLOW
#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <type_traits>
namespace Compiler
{
    template <typename FirstSet, typename Term, typename Other>
    struct get_first_for_term
    {
    private:
        using switch_cond = type_list<Impl::meta_identifier_flag,
                                      Impl::terminal_string_flag,
                                      Impl::empty_sequence_flag>;
        template <template <typename...> typename T, typename... Args>
        struct lazy_template_value
        {
            using type = typename T<Args...>::type::value;
        };
        using switch_vals =
            type_list<lazy_template_value<tavl::tavl_find,
                                          FirstSet,
                                          typename Term::second_type>,
                      lazy_template<tavl::tavl_insert_t,
                                    tavl::empty_node,
                                    type_list<typename Term::second_type>>,
                      identity<tavl::empty_node>>;

    public:
        using type = typename template_switch_t<typename Term::first_type,
                                                switch_cond,
                                                switch_vals,
                                                identity<Other>>::type;
    };
    template <typename First, typename Term, typename Other = void>
    using get_first_for_term_t =
        typename get_first_for_term<First, Term, Other>::type;

    template <typename List>
    struct find_first_not_empty
    {
        // find first symbol that is not an empty_sequence
        using type = List;
    };
    template <typename... Others>
    struct find_first_not_empty<
        type_list<type_pair<Impl::empty_sequence_flag, t_string<>>, Others...>>
    {
        using type = typename find_first_not_empty<type_list<Others...>>::type;
    };
    template <typename Def>
    using start_with_non_empty_t = typename find_first_not_empty<Def>::type;

    template <typename Rules, unsigned K>
    struct first_set
    {
    private:
        /* structure of FIRST_SETS(k)
         * tavl_node<
         *     L,
         *     R,
         *     H,
         *     Identifier(t_string<>),
         *     Values Set(
         *         tavl_node<
         *             L,
         *             R,
         *             H,
         *             first_symbol(type_list<Identifiers>, length of k),
         *             no_meaning
         *         >
         *     )
         * >
         */
        // helper definition for lazy instantiation
        template <typename S1, typename S2>
        using set_addition_helper = set_addition_t<S1, S2, K>;
        // get the terminals at the beginning of a definition
        // used for generating initial list only
        template <typename Definition, unsigned Len>
        struct definition_to_id_list;
        template <typename Id, typename... Others, unsigned Len>
        struct definition_to_id_list<
            type_list<type_pair<Impl::terminal_string_flag, Id>, Others...>,
            Len>
        {
            template <typename NextItems>
            using other_items = type_list_push_front<
                typename definition_to_id_list<NextItems, Len - 1>::type,
                Id>;
            using type = typename std::conditional_t<
                Len == 0,
                identity<type_list<>>,
                lazy_template<other_items, type_list<Others...>>>::type;
        };
        template <typename Id, typename... Others, unsigned Len>
        struct definition_to_id_list<
            type_list<type_pair<Impl::empty_sequence_flag, Id>, Others...>,
            Len>
        {
            static_assert(std::is_same_v<Id, t_string<>>, "bad empty sequence");
            template <typename NextItems>
            using other_items =
                typename definition_to_id_list<NextItems, Len>::type;
            using type = typename std::conditional_t<
                Len == 0,
                identity<type_list<>>,
                lazy_template<other_items, type_list<Others...>>>::type;
        };
        template <typename Flag, typename Id, typename... Others, unsigned Len>
        struct definition_to_id_list<type_list<type_pair<Flag, Id>, Others...>,
                                     Len>
        {
            using type = type_list<>;
        };
        template <unsigned Len>
        struct definition_to_id_list<type_list<>, Len>
        {
            using type = type_list<>;
        };
        template <typename Definition, unsigned Len = K>
        using definition_to_id_list_t =
            typename definition_to_id_list<Definition, Len>::type;
        template <typename Definition>
        using definition_to_id_list_len_k =
            typename definition_to_id_list<Definition, K>::type;

        template <typename Identifier, typename Definitions>
        struct build_initial_set
        {
            // Identifier: t_string<>
            // Definitions: type_list<Definition...>
            // Definition: type_list<Term...>
            // Term: type_pair<tag, t_string<>>
            template <typename T>
            struct for_each_definition
            {
                using type = typename std::conditional_t<
                    has_terminals_only_v<T>,
                    lazy_template<definition_to_id_list_len_k, T>,
                    identity<type_list<>>>::type;
            };
            using generate_first_k_for_terminal_definitions_with_empty =
                type_list_for_each<Definitions, for_each_definition>;
            // type list to TAVL
            using type = tavl::tavl_insert_t<
                tavl::empty_node,
                Identifier,
                type_list_to_tavl<remove_empty_from_list_t<
                    generate_first_k_for_terminal_definitions_with_empty>>>;
        };
        using initial_sets = tavl::tavl_for_each_t<Rules,
                                                   build_initial_set,
                                                   tavl::tavl_union_3,
                                                   tavl::empty_node>;
        template <typename LastSet>
        struct calculate_first_set
        {
            struct skip_flag
            {};

            template <typename Term>
            struct get_set_from_term
            {
                using switch_cond = type_list<Impl::meta_identifier_flag,
                                              Impl::terminal_string_flag,
                                              Impl::empty_sequence_flag>;
                template <template <typename...> typename T, typename... Args>
                struct lazy_template_value
                {
                    using type = typename T<Args...>::type::value;
                };
                using switch_vals = type_list<
                    lazy_template_value<tavl::tavl_find,
                                        LastSet,
                                        typename Term::second_type>,
                    lazy_template<tavl::tavl_insert_t,
                                  tavl::empty_node,
                                  type_list<typename Term::second_type>>>;
                using switch_defs = identity<skip_flag>;
                using type =
                    typename template_switch_t<typename Term::first_type,
                                               switch_cond,
                                               switch_vals,
                                               switch_defs>::type;
            };

            template <typename Term>
            static constexpr bool is_skip_v =
                std::is_same_v<get_first_for_term_t<LastSet, Term, skip_flag>,
                               skip_flag>;
            // specify when to stop the loop for calculating FIRST set
            template <typename Last, typename Curr>
            struct calc_first_set_for_each_definition_stop_cond
            {
                template <typename List>
                struct skip_check
                {
                    template <typename Elem, typename... Elems>
                    static std::integral_constant<
                        bool,
                        !is_skip_v<type_list_top<type_list<Elems...>>>>
                                          test(type_list<Elem, Elems...>);
                    static std::true_type test(...);
                    static constexpr bool value =
                        decltype(test(std::declval<List>()))::value;
                };
                // Last and Curr: type_pair<Set, (part of) Definition>
                static constexpr bool value =
                    (tavl::tavl_is_same_v<typename Curr::second_type,
                                          type_list<>> ||
                     (tavl::tavl_is_same_v<typename Last::first_type,
                                           typename Curr::first_type> &&
                      skip_check<typename Last::second_type>::value) ||
                     tavl::tavl_is_same_v<typename Curr::first_type,
                                          tavl::empty_node>);
            };
            // do calculation
            template <typename Prev>
            struct calc_first_set_for_each_definition_functor
            {
                // Prev: type_pair<Set, (part of) Definition>
                using set  = typename Prev::first_type;
                using list = typename Prev::second_type;

                template <typename Set, typename Curr>
                using lazy_addition_pair =
                    type_pair<set_addition_helper<Set, Curr>,
                              type_list_tail<list>>;

                using curr_set = get_first_for_term_t<LastSet,
                                                      type_list_top<list>,
                                                      skip_flag>;
                using type     = typename std::conditional_t<
                    tavl::tavl_is_same_v<curr_set, skip_flag>,
                    identity<type_pair<set, type_list_tail<list>>>,
                    lazy_template<lazy_addition_pair, set, curr_set>>::type;
            };
            template <typename Set>
            struct calc_first_set_for_each_definition_functor<
                type_pair<Set, type_list<>>>
            {
                using type = type_pair<Set, type_list<>>;
            };
            template <typename Def, typename = void>
            struct calc_first_set_for_each_definition_impl:
                identity<typename do_until<
                    calc_first_set_for_each_definition_stop_cond,
                    calc_first_set_for_each_definition_functor,
                    type_pair<get_first_for_term_t<
                                  LastSet,
                                  type_list_top<start_with_non_empty_t<Def>>,
                                  skip_flag>,
                              type_list_tail<start_with_non_empty_t<Def>>>>::
                             type::first_type>
            {};
            template <typename T>
            struct calc_first_set_for_each_definition_impl<type_list<>, T>:
                identity<tavl::tavl_insert_t<tavl::empty_node, empty_symbol>>
            {};
            template <typename Def>
            using calc_first_set_for_each_definition =
                calc_first_set_for_each_definition_impl<
                    start_with_non_empty_t<Def>>;
            template <typename Def>
            using first_set_for_definition =
                calc_first_set_for_each_definition<Def>;
            template <typename Defs>
            using first_set_for_identifier = type_list_to_tavl_merge<
                type_list_for_each<Defs, first_set_for_definition>>;
            template <typename Id, typename Defs>
            struct for_each_rule
            {
                using type =
                    tavl::tavl_insert_t<tavl::empty_node,
                                        Id,
                                        first_set_for_identifier<Defs>>;
            };
            using type = tavl::tavl_for_each_t<Rules,
                                               for_each_rule,
                                               tavl::tavl_union_3,
                                               tavl::empty_node>;
        };
        template <typename Last, typename Curr>
        struct is_same_debug
        {
            static constexpr bool value = tavl::tavl_is_same_v<Last, Curr>;
        };

    public:
        using type = typename do_until<tavl::tavl_is_same,
                                       calculate_first_set,
                                       initial_sets>::type;
    };
    template <typename Rules, unsigned K>
    using first_set_k = typename first_set<Rules, K>::type;
    template <typename FirstSet, typename Definition, unsigned K>
    struct first_set_for_definition
    {
    private:
        template <typename S1, typename S2>
        using set_addition_helper = set_addition_t<S1, S2, K>;
        template <typename Prev, typename TermList>
        struct calc_first_set
        {
            using cur_term        = type_list_top<TermList>;
            using following_terms = type_list_tail<TermList>;
            using cur_set =
                get_first_for_term_t<FirstSet, cur_term, tavl::empty_node>;
            using calc_result = typename std::conditional_t<
                tavl::tavl_is_same_v<cur_set, tavl::empty_node>,
                identity<Prev>,
                lazy_template<set_addition_helper, Prev, cur_set>>::type;
            using type =
                typename calc_first_set<calc_result, following_terms>::type;
        };
        template <typename Result>
        struct calc_first_set<Result, type_list<>>
        {
            // termination
            using type = Result;
        };
        template <typename LeadingEmptyRemoved, typename>
        struct do_calc
        {
            using type = typename calc_first_set<
                get_first_for_term_t<FirstSet,
                                     type_list_top<LeadingEmptyRemoved>>,
                type_list_tail<LeadingEmptyRemoved>>::type;
        };
        template <typename UNUSED>
        struct do_calc<type_list<>, UNUSED>
        {
            using type = tavl::empty_node;
        };

    public:
        using type =
            typename do_calc<start_with_non_empty_t<Definition>, void>::type;
    };
    template <typename FirstSet, typename Definition, unsigned K>
    using first_set_for_definition_t =
        typename first_set_for_definition<FirstSet, Definition, K>::type;
    template <unsigned K>
    using end_marker = type_list_push_back_n<type_list<>, t_string<'$'>, K>;
    template <typename FollowSet, typename Term, typename Other>
    struct get_follow_for_term
    {
    private:
        using switch_cond = type_list<Impl::meta_identifier_flag,
                                      Impl::terminal_string_flag,
                                      Impl::empty_sequence_flag>;
        template <template <typename...> typename T, typename... Args>
        struct lazy_template_value
        {
            using type = typename T<Args...>::type::value;
        };
        using switch_vals =
            type_list<lazy_template_value<tavl::tavl_find,
                                          FollowSet,
                                          typename Term::second_type>,
                      lazy_template<tavl::tavl_insert_t,
                                    tavl::empty_node,
                                    type_list<typename Term::second_type>>,
                      identity<tavl::empty_node>>;

    public:
        using type = typename std::conditional_t<
            std::is_same_v<typename Term::first_type,
                           Impl::meta_identifier_flag>,
            lazy_template_value<tavl::tavl_find,
                                FollowSet,
                                typename Term::second_type>,
            identity<Other>>::type;
    };
    template <typename First, typename Term, typename Other = void>
    using get_follow_for_term_t =
        typename get_follow_for_term<First, Term, Other>::type;
    template <typename Rules, typename First, typename Start, unsigned K>
    struct follow_set
    {
    private:
        /* structure of FOLLOW_SETS(k)
         * same as FIRST_SETS(k)
         * tavl_node<
         *     L,
         *     R,
         *     H,
         *     Identifier(t_string<>),
         *     Values Set(
         *         tavl_node<
         *             L,
         *             R,
         *             H,
         *             first_symbol(type_list<Identifiers>, length of k),
         *             no_meaning
         *         >
         *     )
         * >
         */

        using init_set = tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Start,
            tavl::tavl_node<tavl::empty_node,
                            tavl::empty_node,
                            0,
                            end_marker<K>,
                            std::integral_constant<bool, true>>>;
        struct skip_flag;
        template <typename S1, typename S2>
        using set_addition_helper = set_addition_t<S1, S2, K>;
        template <typename LastFollow, typename Definition, typename Identifier>
        struct do_calc_follow
        {
            using first_symbols_after_current =
                first_set_for_definition_t<First, Definition, K>;
            using current_follow_node =
                tavl::tavl_find_t<LastFollow, Identifier>;
            template <typename FollowNode>
            struct lazy_helper
            {
                using type = typename std::conditional_t<
                    tavl::tavl_is_same_v<first_symbols_after_current,
                                         tavl::empty_node>,
                    identity<typename FollowNode::value>,
                    lazy_template<set_addition_helper,
                                  first_symbols_after_current,
                                  typename FollowNode::value>>::type;
            };
            using type = typename std::conditional_t<
                tavl::tavl_is_same_v<current_follow_node, tavl::empty_node>,
                lazy_template<identity, first_symbols_after_current>,
                lazy_template<lazy_helper, current_follow_node>>::type::type;
        };
        template <typename LastFollow, typename Definition, typename Identifier>
        using do_calc_follow_t =
            typename do_calc_follow<LastFollow, Definition, Identifier>::type;
        template <typename L, typename C, typename R>
        using tavl_union_with_merge_3 =
            tavl::tavl_union_with_func<tavl_merge_for_union, L, C, R>;
        template <typename Last>
        struct calc_follow
        {
            template <typename Identifier, typename Defs>
            struct for_each_rule
            {
                template <typename PartOfDef, typename NewSet>
                struct for_each_term
                {
                    using current_term = type_list_top<PartOfDef>;
                    using next_defs    = type_list_tail<PartOfDef>;
                    template <typename T>
                    struct for_nonterminal_only
                    {
                        using type = NewSet;
                    };
                    template <typename Id>
                    struct for_nonterminal_only<
                        type_pair<Impl::meta_identifier_flag, Id>>
                    {
                        using new_follow_for_current_term = tavl::tavl_remove_t<
                            do_calc_follow_t<Last, next_defs, Identifier>,
                            empty_symbol>;
                        using follow_before = std::conditional_t<
                            tavl::tavl_contain_v<NewSet, Id>,
                            typename tavl::tavl_find_t<NewSet, Id>::value,
                            tavl::empty_node>;
                        using after_union =
                            tavl::tavl_union_t<follow_before,
                                               new_follow_for_current_term>;
                        using type =
                            tavl::tavl_update_t<NewSet, Id, after_union>;
                    };
                    using new_set =
                        typename for_nonterminal_only<current_term>::type;
                    using type =
                        typename for_each_term<next_defs, new_set>::type;
                };
                template <typename NewSet>
                struct for_each_term<type_list<>, NewSet>
                {
                    using type = NewSet;
                };
                template <typename Definitions, typename NewSet>
                struct for_each_definition
                {
                    using current_def = type_list_top<Definitions>;
                    using next_defs   = type_list_tail<Definitions>;
                    using type        = typename for_each_definition<
                        next_defs,
                        typename for_each_term<current_def,
                                               NewSet>::type>::type;
                };
                template <typename NewSet>
                struct for_each_definition<type_list<>, NewSet>
                {
                    using type = NewSet;
                };
                using type = typename for_each_definition<Defs, Last>::type;
            };

            using type = tavl::tavl_union_with_func_t<
                tavl_merge_for_union,
                Last,
                tavl::tavl_for_each_t<Rules,
                                      for_each_rule,
                                      tavl_union_with_merge_3,
                                      tavl::empty_node>>;
        };
        template <typename Id, typename Follows>
        struct remove_less_than_k
        {
            template <typename Symbol, typename>
            struct for_each_symbol
            {
                using type = std::conditional_t<
                    type_list_size<Symbol> == K,
                    tavl::tavl_insert_t<tavl::empty_node, Symbol>,
                    tavl::empty_node>;
            };
            using type =
                tavl::tavl_insert_t<tavl::empty_node,
                                    Id,
                                    tavl::tavl_for_each_t<Follows,
                                                          for_each_symbol,
                                                          tavl::tavl_union_3,
                                                          tavl::empty_node>>;
        };
        using origin_type =
            typename do_until<tavl::tavl_is_same, calc_follow, init_set>::type;

    public:
        using type = tavl::tavl_for_each_t<origin_type,
                                           remove_less_than_k,
                                           tavl::tavl_union_3,
                                           tavl::empty_node>;
    };
    template <typename Rules, typename First, typename Start, unsigned K>
    using follow_set_t = typename follow_set<Rules, First, Start, K>::type;
} // namespace Compiler
#endif
