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

#ifndef PARSER_IN_STRING_MATCHER_LEXER
#define PARSER_IN_STRING_MATCHER_LEXER

#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "Common/log.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include "fsm.hpp"
#include "lexer_interface.hpp"
#include "regex.hpp"
#include <cassert>
#include <functional>
#include <istream>
#include <queue>
#include <string>
#include <type_traits>
#include <utility>
namespace Compiler
{
    namespace Impl
    {
        // main data structures:
        // FA:
        // tavl_node<L, R, H,
        //     value_t<DFA_STATE>,
        //     tavl_node<L, R, H,
        //         value_t<EDGE>,
        //         tavl_node<L, R, H, value_t<DFA_STATE>>
        //     >
        // >
        // Finite Set(NFA):
        // tavl_node<L, R, H,
        //     value_t<NFA_STATE>,
        //     MARKER
        // >

        // subset construction method
        // set of ||d||:
        // tavl_node<L, R, H,
        //     tavl_node<L, R, H, value_t<NFA_STATE>>
        // >
        // B (buds):
        // type_list<tavl_node<L, R, H, value_t<NFA_STATE>>>
        // helper functions
        template <char16_t l, typename N, typename NFA>
        struct calc_closure
        {
            template <typename S, typename>
            struct for_each_state
            {
                using outs = tavl_find_empty_node<NFA, S>;
                using type = std::conditional_t<
                    tavl::tavl_contain_v<outs, value_wrapper<l>>,
                    typename tavl::tavl_find_t<outs, value_wrapper<l>>::value,
                    tavl::empty_node>;
            };
            using type = tavl::tavl_for_each_t<N,
                                               for_each_state,
                                               tavl::tavl_union_3,
                                               tavl::empty_node>;
        };
        template <char16_t l, typename N>
        struct calc_closure<l, N, tavl::empty_node>
        {
            using type = tavl::empty_node;
        };
        template <char16_t l, typename N, typename NFA>
        using calc_closure_t = typename calc_closure<l, N, NFA>::type;
        template <typename N, typename NFA>
        struct empty_closure
        {
        private:
            template <typename States>
            using calc =
                tavl::tavl_union<States,
                                 calc_closure_t<EMPTY_TRANS_EDGE, States, NFA>>;

        public:
            using type = typename do_until<tavl::tavl_is_same, calc, N>::type;
            static_assert(!tavl::is_empty_node_v<type>);
        };
        template <typename N, typename NFA>
        using empty_closure_t = typename empty_closure<N, NFA>::type;
        template <char16_t l, typename N, typename NFA>
        struct l_closure
        {
            using type = empty_closure_t<calc_closure_t<l, N, NFA>, NFA>;
        };
        template <char16_t l, typename N, typename NFA>
        using l_closure_t = typename l_closure<l, N, NFA>::type;
        template <typename N, typename Finite, typename NFinite>
        struct finite_check
        {
            template <typename S, typename>
            struct for_each_state_in_n
            {
                using type =
                    std::bool_constant<tavl::tavl_contain_v<NFinite, S>>;
            };
            static constexpr bool is_finite =
                tavl::tavl_for_each_t<N,
                                      for_each_state_in_n,
                                      logical_or_merger,
                                      std::false_type>::value;
            using type = typename std::conditional_t<
                !tavl::tavl_contain_v<Finite, N> && is_finite,
                lazy_template<set_try_insert_t, Finite, N>,
                identity<Finite>>::type;
        };
        template <typename N, typename Finite, typename NFinite>
        using finite_check_t = typename finite_check<N, Finite, NFinite>::type;
        template <typename N, typename NFinite>
        using get_target_from_nfa_finite = std::conditional_t<
            tavl::tavl_contain_v<NFinite, N>,
            tavl::kv_pair<typename tavl::tavl_find_t<NFinite, N>::value>,
            tavl::empty_node>;
        template <typename Finite, typename NFinite>
        struct add_target_to_finite
        {
            template <typename S, typename>
            struct for_each_nstate
            {
                using type = get_target_from_nfa_finite<S, NFinite>;
            };
            template <typename F, typename>
            struct for_each_finite
            {
                using type =
                    tavl::kv_pair<F,
                                  tavl::tavl_for_each_t<F,
                                                        for_each_nstate,
                                                        tavl::tavl_union_3,
                                                        tavl::empty_node>>;
            };
            using type = tavl::tavl_for_each_t<Finite,
                                               for_each_finite,
                                               tavl::tavl_union_3,
                                               tavl::empty_node>;
        };
        template <typename NFA, typename Finite>
        struct subset_construction
        {
        private:
            using start_set =
                empty_closure_t<tavl::kv_pair<value_wrapper<START_STATE>>, NFA>;
            using init_d_set = tavl::kv_pair<start_set>;
            using init_finite =
                finite_check_t<start_set, tavl::empty_node, Finite>;
            using init_buds = type_list<start_set>;
            template <typename Before>
            struct do_sc
            {
                using dfa    = type_list_get<Before, 0>;
                using finite = type_list_get<Before, 1>;
                using d_set  = type_list_get<Before, 2>;
                using buds   = type_list_get<Before, 3>;
                static_assert(!std::is_same_v<buds, type_list<>>);
                using current_state = type_list_top<buds>;
                template <typename BeforeDFDB, typename N>
                struct for_each_state_in_d
                {
                    using outs = tavl_find_empty_node<NFA, N>;
                    template <typename SubBeforeDFDB, typename L>
                    struct for_each_input_symbol
                    {
                        using before_dfa    = type_list_get<SubBeforeDFDB, 0>;
                        using before_finite = type_list_get<SubBeforeDFDB, 1>;
                        using before_d_set  = type_list_get<SubBeforeDFDB, 2>;
                        using before_buds   = type_list_get<SubBeforeDFDB, 3>;
                        using d_dash =
                            l_closure_t<L::value, current_state, NFA>;
                        using new_finite = typename std::conditional_t<
                            tavl::tavl_contain_v<before_d_set, d_dash>,
                            identity<before_finite>,
                            lazy_template<finite_check_t,
                                          d_dash,
                                          before_finite,
                                          Finite>>::type;
                        using new_d_set =
                            set_try_insert_t<before_d_set, d_dash>;
                        using new_bud = typename std::conditional_t<
                            tavl::tavl_contain_v<before_d_set, d_dash>,
                            identity<before_buds>,
                            lazy_template<type_list_push_front,
                                          before_buds,
                                          d_dash>>::type;
                        using new_dfa =
                            fa_add_edge_t<current_state, L, d_dash, before_dfa>;
                        using type =
                            type_list<new_dfa, new_finite, new_d_set, new_bud>;
                    };
                    template <typename SubBeforeDFDB>
                    struct for_each_input_symbol<
                        SubBeforeDFDB,
                        value_wrapper<EMPTY_TRANS_EDGE>>
                    {
                        using type = SubBeforeDFDB;
                    };
                    using type = tavl::tavl_for_each_middle_order_t<
                        outs,
                        get_key,
                        for_each_input_symbol,
                        BeforeDFDB>;
                };
                using type = tavl::tavl_for_each_middle_order_t<
                    current_state,
                    get_key,
                    for_each_state_in_d,
                    type_list<dfa, finite, d_set, type_list_tail<buds>>>;
            };
            template <typename, typename Curr>
            struct is_empty
            {
                static constexpr bool value =
                    std::is_same_v<type_list_get<Curr, 3>, type_list<>>;
            };
            using dfdb = typename do_until<is_empty,
                                           do_sc,
                                           type_list<tavl::empty_node,
                                                     init_finite,
                                                     init_d_set,
                                                     init_buds>>::type;

        public:
            using start = start_set;
            using dfa   = type_list_get<dfdb, 0>;
            using finite_set =
                typename add_target_to_finite<type_list_get<dfdb, 1>,
                                              Finite>::type;
            using d_set = type_list_get<dfdb, 2>;
        };
        // parse all rules and generate DFA
        template <typename NFAParser,
                  bool add_to_finite = false,
                  typename Finite    = tavl::empty_node,
                  typename Marker    = void>
        struct finite_wrapper: NFAParser
        {
            using finite = typename std::conditional_t<
                add_to_finite,
                lazy_template<set_try_insert_t,
                              Finite,
                              value_wrapper<NFAParser::end_state>,
                              Marker>,
                identity<Finite>>::type;
        };
        template <typename TSTR, unsigned Next, unsigned Start, typename NFA>
        struct t_string_to_nfa
        {
        private:
            static_assert(is_t_string_v<TSTR>);
            template <unsigned next, typename T, typename N>
            struct for_each_char;
            template <unsigned next, char ch, char... chs, typename N>
            struct for_each_char<next, t_string<ch, chs...>, N>
            {
                using nfa = fa_add_edge_t<value_wrapper<next>,
                                          value_wrapper<ch>,
                                          value_wrapper<next + 1>,
                                          N>;
                static constexpr unsigned next_state_used =
                    for_each_char<next + 1, t_string<chs...>, nfa>::
                        next_state_used;
                using type = typename for_each_char<next + 1,
                                                    t_string<chs...>,
                                                    nfa>::type;
            };
            template <unsigned next, typename N>
            struct for_each_char<next, t_string<>, N>
            {
                static constexpr unsigned next_state_used = next;
                using type                                = N;
            };

        public:
            static constexpr unsigned end_state =
                for_each_char<Next, TSTR, NFA>::next_state_used;
            using nfa =
                fa_add_edge_t<value_wrapper<Start>,
                              value_wrapper<EMPTY_TRANS_EDGE>,
                              value_wrapper<Next>,
                              typename for_each_char<Next, TSTR, NFA>::type>;
        };
        template <typename Symbol, typename Data>
        struct parse_default
        {
            using type        = Data;
            using replacement = Symbol;
        };
        template <typename Symbol, typename Data>
        struct parse_terminal_string;
        template <typename Symbol, typename NFA, typename Finite, typename Next>
        struct parse_terminal_string<Symbol, type_list<NFA, Finite, Next>>
        {
        private:
            using TSTR = typename Symbol::second_type;
            using tstr_to_nfa =
                t_string_to_nfa<TSTR, Next::value, START_STATE, NFA>;
            using result =
                finite_wrapper<tstr_to_nfa,
                               true,
                               Finite,
                               type_pair<Impl::terminal_string_flag, TSTR>>;

        public:
            using nfa                      = typename result::nfa;
            using finite                   = typename result::finite;
            static constexpr unsigned next = result::end_state + 1;

            using type        = type_list<nfa, finite, value_wrapper<next>>;
            using replacement = type_pair<terminal_string_flag, TSTR>;
        };
        template <typename Special, typename Data>
        struct parse_special_sequence
        {
            using type = Data;
            using replacement =
                type_pair<terminal_string_flag,
                          decltype(concat(
                              std::declval<special_sequence_leading>(),
                              std::declval<typename Special::second_type>()))>;
        };
        template <typename Def, typename Data>
        struct do_parse_lexer_rule
        {
            using type =
                typename lazy_template<identity,
                                       Def>::type::unsupported_term_here;
        };
        template <typename Data>
        struct do_parse_lexer_rule<type_list<>, Data>
        {
            using type = Data;
        };
        template <typename V,
                  typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<terminal_string_flag, V>, Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            static constexpr unsigned tstr_end =
                t_string_to_nfa<V, Next::value, Start::value, NFA>::end_state;
            using nfa =
                typename t_string_to_nfa<V, Next::value, Start::value, NFA>::
                    nfa;

            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<nfa,
                          Finite,
                          value_wrapper<tstr_end + 1>,
                          value_wrapper<tstr_end>>>::type;
        };

        template <typename... Terms, typename... Def, typename Data>
        struct do_parse_lexer_rule<type_list<type_list<Terms...>, Def...>, Data>
        {
            using first_def =
                typename do_parse_lexer_rule<type_list<Terms...>, Data>::type;

            using nfa    = type_list_get<first_def, 0>;
            using finite = type_list_get<first_def, 1>;
            using next   = type_list_get<first_def, 2>;
            // start state is same
            using type = typename do_parse_lexer_rule<
                type_list<Def...>,
                type_list<nfa, finite, next, type_list_get<Data, 3>>>::type;
        };
        template <typename V,
                  typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<grouped_sequence_flag, V>, Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using grouped_data = typename do_parse_lexer_rule<
                V,
                type_list<NFA, Finite, Next, Start>>::type;
            using nfa    = type_list_get<grouped_data, 0>;
            using finite = type_list_get<grouped_data, 1>;
            using next   = type_list_get<grouped_data, 2>;
            using type   = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_edge_t<value_wrapper<next::value - 1>,
                                        value_wrapper<EMPTY_TRANS_EDGE>,
                                        next,
                                        nfa>,
                          finite,
                          value_wrapper<next::value + 1>,
                          next>>::type;
        };
        template <typename V,
                  typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<optional_sequence_flag, V>, Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using grouped_data = typename do_parse_lexer_rule<
                V,
                type_list<NFA, Finite, Next, Start>>::type;
            using nfa     = type_list_get<grouped_data, 0>;
            using finite  = type_list_get<grouped_data, 1>;
            using next    = type_list_get<grouped_data, 2>;
            using new_nfa = fa_add_edge_t<Start,
                                          value_wrapper<EMPTY_TRANS_EDGE>,
                                          next,
                                          nfa>;
            using type    = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<new_nfa,
                          finite,
                          value_wrapper<next::value + 1>,
                          next>>::type;
        };
        template <typename V,
                  typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<repeated_sequence_flag, V>, Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using grouped_data = typename do_parse_lexer_rule<
                V,
                type_list<NFA, Finite, value_wrapper<Next::value + 1>, Next>>::
                type;
            using nfa     = type_list_get<grouped_data, 0>;
            using finite  = type_list_get<grouped_data, 1>;
            using next    = type_list_get<grouped_data, 2>;
            using new_nfa = fa_add_edge_t<
                value_wrapper<next::value - 1>,
                value_wrapper<EMPTY_TRANS_EDGE>,
                next,
                fa_add_edge_t<
                    value_wrapper<next::value - 1>,
                    value_wrapper<EMPTY_TRANS_EDGE>,
                    Next,
                    fa_add_edge_t<Start,
                                  value_wrapper<EMPTY_TRANS_EDGE>,
                                  next,
                                  fa_add_edge_t<Start,
                                                value_wrapper<EMPTY_TRANS_EDGE>,
                                                Next,
                                                nfa>>>>;
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<new_nfa,
                          finite,
                          value_wrapper<next::value + 1>,
                          next>>::type;
        };
        template <typename V, typename... Rest, typename Data>
        struct do_parse_lexer_rule<
            type_list<type_pair<empty_sequence_flag, V>, Rest...>,
            Data>: do_parse_lexer_rule<type_list<Rest...>, Data>
        {};
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("alpha")>,
                      Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_chars_t<
                              Start,
                              Next,
                              'A',
                              'Z' + 1,
                              fa_add_chars_t<Start, Next, 'a', 'z' + 1, NFA>>,
                          Finite,
                          value_wrapper<Next::value + 1>,
                          Next>>::type;
        };
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("upper")>,
                      Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_chars_t<Start, Next, 'A', 'Z' + 1, NFA>,
                          Finite,
                          value_wrapper<Next::value + 1>,
                          Next>>::type;
        };
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("lower")>,
                      Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_chars_t<Start, Next, 'a', 'z' + 1, NFA>,
                          Finite,
                          value_wrapper<Next::value + 1>,
                          Next>>::type;
        };
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("hex")>, Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_chars_t<
                              Start,
                              Next,
                              '0',
                              '9' + 1,
                              fa_add_chars_t<Start, Next, 'a', 'f' + 1, NFA>>,
                          Finite,
                          value_wrapper<Next::value + 1>,
                          Next>>::type;
        };
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("number")>,
                      Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_chars_t<Start, Next, '0', '9' + 1, NFA>,
                          Finite,
                          value_wrapper<Next::value + 1>,
                          Next>>::type;
        };
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("oct")>, Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_chars_t<Start, Next, '0', '7' + 1, NFA>,
                          Finite,
                          value_wrapper<Next::value + 1>,
                          Next>>::type;
        };
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("alnum")>,
                      Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<
                    fa_add_chars_t<
                        Start,
                        Next,
                        '0',
                        '9' + 1,
                        fa_add_chars_t<
                            Start,
                            Next,
                            'A',
                            'Z' + 1,
                            fa_add_chars_t<Start, Next, 'a', 'z' + 1, NFA>>>,
                    Finite,
                    value_wrapper<Next::value + 1>,
                    Next>>::type;
        };
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("any")>, Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_chars_t<Start, Next, 1, 128, NFA>,
                          Finite,
                          value_wrapper<Next::value + 1>,
                          Next>>::type;
        };
        template <typename... Rest,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, _TSTR("print")>,
                      Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                type_list<fa_add_chars_t<Start, Next, 32, 127, NFA>,
                          Finite,
                          value_wrapper<Next::value + 1>,
                          Next>>::type;
        };
        template <typename... Rest,
                  typename Regex,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct do_parse_lexer_rule<
            type_list<type_pair<special_sequence_flag, Regex>, Rest...>,
            type_list<NFA, Finite, Next, Start>>
        {
            // regex in special sequence
            static_assert(first_character_s<Regex>::value == '@',
                          "Special Sequence given is not regex");
            static_assert(type_list_size<parse_regex_t<ignore_one_s_t<Regex>,
                                                       NFA,
                                                       Finite,
                                                       Next,
                                                       Start>> == 4,
                          "Redundant ')'");
            using type = typename do_parse_lexer_rule<
                type_list<Rest...>,
                parse_regex_t<ignore_one_s_t<Regex>,
                              NFA,
                              Finite,
                              Next,
                              Start>>::type;
        };
        template <typename Definition, typename Data>
        struct parse_lexer_rule
        {
            using def = type_list_tail<Definition>;

            using type =
                type_list<type_pair<terminal_string_flag,
                                    // Here we add an special prefix '", which
                                    // cannot be found in any types of terminal
                                    // string to tell the Lexer & Parser that
                                    // there is an user-defined lexer rule.
                                    decltype(add_one<'\''>(add_one<'"'>(
                                        std::declval<typename type_list_top<
                                            Definition>::second_type>())))>>;
            using do_parse = typename do_parse_lexer_rule<
                def,
                type_list_push_back<Data, value_wrapper<START_STATE>>>::type;
            using nfa    = type_list_get<do_parse, 0>;
            using next   = type_list_get<do_parse, 2>;
            using finite = set_try_insert_t<type_list_get<do_parse, 1>,
                                            value_wrapper<next::value - 1>,
                                            type_list_top<Definition>>;
            using data   = type_list<nfa, finite, next>;
        };
        template <typename Def, typename Data>
        struct for_each_terminal_or_special_in_non_lexer_defs
        {
            using Target = type_list_top<Def>;
            using switch_cond =
                type_list<terminal_string_flag, special_sequence_flag>;
            using switch_vals =
                type_list<lazy_template<parse_terminal_string, Target, Data>,
                          lazy_template<parse_special_sequence, Target, Data>>;
            using result = typename template_switch_t<
                typename Target::first_type,
                switch_cond,
                switch_vals,
                lazy_template<parse_default, Target, Data>>::type;
            using new_data   = typename result::type;
            using new_symbol = typename result::replacement;

            using type = type_list_push_front<
                typename for_each_terminal_or_special_in_non_lexer_defs<
                    type_list_tail<Def>,
                    new_data>::type,
                new_symbol>;
            using data =
                typename for_each_terminal_or_special_in_non_lexer_defs<
                    type_list_tail<Def>,
                    new_data>::data;
        };
        template <typename Data>
        struct for_each_terminal_or_special_in_non_lexer_defs<type_list<>, Data>
        {
            using type = type_list<>;
            using data = Data;
        };
        template <typename Definitions, typename Data>
        struct for_each_definition
        {
            using current = type_list_top<Definitions>;
            using parser  = typename std::conditional_t<
                is_lexer_rule<current>,
                lazy_template<parse_lexer_rule, current, Data>,
                lazy_template<for_each_terminal_or_special_in_non_lexer_defs,
                              current,
                              Data>>::type;

            using type = type_list_push_front<
                typename for_each_definition<type_list_tail<Definitions>,
                                             typename parser::data>::type,
                typename parser::type>;
            using data =
                typename for_each_definition<type_list_tail<Definitions>,
                                             typename parser::data>::data;
        };
        template <typename Data>
        struct for_each_definition<type_list<>, Data>
        {
            using type = type_list<>;
            using data = Data;
        };
        template <typename RD, typename Pack>
        struct for_each_rule
        {
            using data        = typename RD::second_type;
            using symbol      = typename Pack::key;
            using definitions = typename Pack::value;
            using def_parser  = for_each_definition<definitions, data>;

            using new_data  = typename def_parser::data;
            using new_rules = tavl::tavl_update_t<typename RD::first_type,
                                                  symbol,
                                                  typename def_parser::type>;
            using type      = type_pair<new_rules, new_data>;
        };

        template <char... chs>
        inline constexpr std::true_type
            lexer_rule_str(t_string<'\'', '"', 'l', 'e', 'x', 'e', 'r', chs...>)
        {
            return {};
        }
        inline constexpr std::false_type lexer_rule_str(...)
        {
            return {};
        }
        template <char... chs>
        inline constexpr bool is_lexer_rule_str_v =
            decltype(lexer_rule_str(std::declval<t_string<chs...>>()))::value;
        template <char... chs>
        inline constexpr std::true_type special_str(t_string<'\'', '"', chs...>)
        {
            return {};
        }
        inline constexpr std::false_type special_str(...)
        {
            return {};
        }
        template <char... chs>
        inline constexpr bool is_special_str_v =
            decltype(special_str(std::declval<t_string<chs...>>()))::value;
        template <char... chs>
        inline constexpr t_string<chs...>
            remove_leading_indicator(t_string<'\'', '"', chs...>)
        {
            return {};
        }
        template <char... chs>
        using remove_leading_indicator_t = decltype(
            Impl::remove_leading_indicator(std::declval<t_string<chs...>>()));
    } // namespace Impl
    template <typename Rules>
    class lexer: public lexer_interface
    {
    private:
        using rewrite_rules = typename tavl::tavl_for_each_middle_order_t<
            Rules,
            postpone_to_merge,
            Impl::for_each_rule,
            type_pair<tavl::empty_node,
                      // data
                      type_list<tavl::empty_node,
                                tavl::empty_node,
                                value_wrapper<START_STATE + 1>>>>;
        using new_rules  = typename rewrite_rules::first_type;
        using data       = typename rewrite_rules::second_type;
        using nfa        = type_list_get<data, 0>;
        using finite_nfa = type_list_get<data, 1>;
        using next_state = type_list_get<data, 2>;

        using sc = Impl::subset_construction<nfa, finite_nfa>;

        template <typename Set>
        void dump_set(std::string& str) const
        {
            if constexpr (!std::is_same_v<Set, tavl::empty_node>)
            {
                dump_set<typename Set::left>(str);
                if constexpr (tavl::tavl_contain_v<finite_nfa,
                                                   typename Set::key>)
                {
                    str += "[" + std::to_string(Set::key::value) + "] ";
                }
                else
                {
                    str += std::to_string(Set::key::value) + " ";
                }
                dump_set<typename Set::right>(str);
            }
        }

        template <typename Ins>
        void dump_ins(std::string& str) const
        {
            if constexpr (!std::is_same_v<Ins, tavl::empty_node>)
            {
                dump_ins<typename Ins::left>(str);
                str += "\t\t{";
                dump_set<typename Ins::key>(str);
                str.pop_back();
                str += "}\n";
                dump_ins<typename Ins::right>(str);
            }
        }

        template <typename Labels>
        void dump_label(std::string& str) const
        {
            if constexpr (!std::is_same_v<Labels, tavl::empty_node>)
            {
                dump_label<typename Labels::left>(str);
                using namespace std::string_literals;
                str += "\t";
                if constexpr (Labels::key::value < 255)
                {
                    str += static_cast<char>(Labels::key::value);
                }
                else
                {
                    str += std::to_string(static_cast<int>(Labels::key::value));
                }
                str += "\n";
                dump_ins<typename Labels::value>(str);
                dump_label<typename Labels::right>(str);
            }
        }

        template <typename FA>
        void dump(std::string& str) const
        {
            if constexpr (!std::is_same_v<FA, tavl::empty_node>)
            {
                dump<typename FA::left>(str);
                str += "{";
                dump_set<typename FA::key>(str);
                str.pop_back();
                str += "}\n";
                dump_label<typename FA::value>(str);
                dump<typename FA::right>(str);
            }
        }
        class lookahead_guard
        {
        public:
            lookahead_guard(lexer& lexer): _lexer(lexer)
            {
                _before           = _lexer._input.get().tellg();
                _lexer._skip_comp = true;
            }
            ~lookahead_guard()
            {
                _lexer._skip_comp = false;
                _lexer._input.get().seekg(_before);
            }

        private:
            lexer&                 _lexer;
            std::istream::int_type _before;
        };
        class skip_compare
        {
        public:
            skip_compare(lexer& lexer): _lexer(lexer)
            {
                _lexer._skip_comp = true;
            }
            ~skip_compare()
            {
                _lexer._skip_comp = false;
            }

        private:
            lexer& _lexer;
        };

    public:
        using start_state = typename sc::start;
        using dfa         = typename sc::dfa;
        using dfa_state   = typename sc::d_set;
        using finite_dfa  = typename sc::finite_set;
        using type        = new_rules;

        lexer(std::istream&                                      input,
              parser_interface                                   parser,
              std::unordered_map<std::string, lexer_listener_t>& listeners,
              const std::string& skip = " \f\n\r\t\v"):
            lexer_interface(parser, listeners),
            _input(std::ref(input)),
            _skip(skip)
        {}
        template <typename Str>
        std::string readNext()
        {
            auto& input = _input.get();
            auto  ch    = input.peek();
            while ((ch = input.peek()) != EOF &&
                   _skip.find(ch) != std::string::npos)
            {
                input.get();
            }
            return parse(Str{});
        }
        std::pair<std::vector<std::string>, std::string> read_any() override
        {
            skip_compare guard{*this};
            std::string  no_meaning;
            _parse_types.clear();
            do_parse<void, start_state>(no_meaning);
            remove_prefix_of_types();
            return std::make_pair(_parse_types, no_meaning);
        }
        template <unsigned K>
        std::vector<std::vector<std::string>> lookahead()
        {
            std::vector<std::vector<std::string>> result{K};
            lookahead_guard                       guard{*this};
            std::string                           no_meaning;
            _parse_types.clear();
            for (int i = 0; i < K; ++i)
            {
                if (do_parse<void, start_state>(no_meaning))
                {
                    result[i] = _parse_types;
                    _parse_types.clear();
                }
                else
                {
                    break;
                }
            }
            return result;
        }
        std::string dump() const
        {
            std::string output;
            dump<dfa>(output);
            return output;
        }
        std::istream& get_stream() override
        {
            return _input;
        }
        bool eof() const
        {
            auto& input = _input.get();
            auto  ch    = input.peek();
            while ((ch = input.peek()) != EOF &&
                   _skip.find(ch) != std::string::npos)
            {
                input.get();
            }
            return input.eof();
        }

    private:
        template <char... chs>
        std::string parse(t_string<chs...>)
        {
            std::string str;
            if constexpr (Impl::is_lexer_rule_str_v<chs...>)
            {
                using type =
                    type_pair<Impl::special_sequence_flag,
                              Impl::remove_leading_indicator_t<chs...>>;
                if (do_parse<type, start_state>(str))
                {
                    return str;
                }
                else
                {
                    // error handling here
                    return "";
                }
            }
            else if constexpr (Impl::is_special_str_v<chs...>)
            {
                using real_str = Impl::remove_leading_indicator_t<chs...>;
                // TODO: call user function <real_str>
                return call_listener<real_str>();
            }
            else
            {
                if constexpr (tavl::is_empty_node_v<dfa>)
                {
                    return "";
                }
                else
                {
                    using type =
                        type_pair<Impl::terminal_string_flag, t_string<chs...>>;
                    if (do_parse<type, start_state>(str))
                    {
                        return str;
                    }
                    else
                    {
                        // error handling here
                        return "";
                    }
                }
            }
        }
        template <typename Marker, typename State>
        bool do_parse(std::string& str)
        {
            PRINT_LOG("lexer-parsing: " + str);
            constexpr bool is_finite = tavl::tavl_contain_v<finite_dfa, State>;
            if constexpr (tavl::is_empty_node_v<State>)
            {
                // invalid state
                return false;
            }
            else if constexpr (tavl::tavl_contain_v<dfa, State>)
            {
                auto& input      = _input.get();
                auto  len        = str.size();
                auto  stream_pos = input.tellg();
                if (auto ch = input.peek();
                    ch != std::istream::traits_type::eof() &&
                    move_forward<Marker,
                                 typename tavl::tavl_find_t<dfa, State>::value>(
                        str, ch))
                {
                    return true;
                }
                else
                {
                    // fallback
                    str = str.substr(0, len);
                    input.seekg(stream_pos);
                    PRINT_LOG("lexer-fallback: " + str);
                    if constexpr (is_finite)
                    {
                        if constexpr (std::is_same_v<Marker, void>)
                        {
                            if (_skip_comp)
                            {
                                // lookahead
                                lookahead_types<
                                    typename tavl::tavl_find_t<finite_dfa,
                                                               State>::value>();
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else
                        {
                            return tavl::tavl_contain_v<
                                typename tavl::tavl_find_t<finite_dfa,
                                                           State>::value,
                                Marker>;
                        }
                    }
                    return false;
                }
            }
            else
            {
                // this is the last state
                if constexpr (is_finite)
                {
                    if constexpr (std::is_same_v<Marker, void>)
                    {
                        if (_skip_comp)
                        {
                            // lookahead
                            lookahead_types<
                                typename tavl::tavl_find_t<finite_dfa,
                                                           State>::value>();
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return tavl::tavl_contain_v<
                            typename tavl::tavl_find_t<finite_dfa,
                                                       State>::value,
                            Marker>;
                    }
                }
                return false;
            }
        }
        template <typename Marker, typename Edges>
        bool move_forward(std::string& str, char ch)
        {
            PRINT_LOG("lexer-move_forward: " + str + ch);
            if constexpr (tavl::is_empty_node_v<Edges>)
            {
                // cannot accept ch from this state
                return false;
            }
            else
            {
                int comp_result = ch - Edges::key::value;
                if (comp_result == 0)
                {
                    str += _input.get().get();
                    return do_parse<Marker, typename Edges::value::key>(str);
                }
                else if (comp_result < 0)
                {
                    return move_forward<Marker, typename Edges::left>(str, ch);
                }
                else
                {
                    return move_forward<Marker, typename Edges::right>(str, ch);
                }
            }
        }
        template <typename T>
        void lookahead_types()
        {
            if constexpr (tavl::is_empty_node_v<T>)
            {
                return;
            }
            else
            {
                lookahead_types<typename T::left>();
                if constexpr (std::is_same_v<typename T::key::first_type,
                                             Impl::special_sequence_flag>)
                {
                    _parse_types.emplace_back(
                        "'\"" + to_string(typename T::key::second_type{}));
                }
                else
                {
                    _parse_types.emplace_back(
                        to_string(typename T::key::second_type{}));
                }
                lookahead_types<typename T::right>();
            }
        }
        void remove_prefix_of_types()
        {
            for (auto& str : _parse_types)
            {
                if (str[0] == '\'' && str[0] == '"')
                {
                    // lexer rule
                    assert(str.substr(0, 8) == "'\"lexer ");
                    str = str.substr(8);
                }
            }
        }
        std::reference_wrapper<std::istream> _input;
        std::string _skip; // characters can be ignored between symbols
        bool        _skip_comp{false}; // for lookahead
        std::vector<std::string> _parse_types;
    };
    inline std::string
        print_lookahead(const std::vector<std::vector<std::string>>& lookaheads)
    {
        std::string result;
        for (const auto& v : lookaheads)
        {
            result += "[";
            for (const auto& t : v)
            {
                result += t + " ";
            }
            result.back() = ']';
        }
        return result;
    }
} // namespace Compiler
#endif
