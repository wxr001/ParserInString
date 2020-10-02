// ParserInString -*- C++ -*-

// Copyright (C) 2020  Scott Brown

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

#ifndef PARSER_IN_STRING_LEXER_REGEX
#define PARSER_IN_STRING_LEXER_REGEX

#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include "fsm.hpp"
#include <type_traits>
namespace Compiler
{
    namespace Impl
    {
        template <typename T>
        struct invalid_optional_sequence;
        template <typename Regex,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct parse_regex;
        template <typename Regex,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct duplication
        {
            using type = typename parse_regex<Regex,
                                              NFA,
                                              Finite,
                                              value_wrapper<Next::value + 1>,
                                              Next>::type;
        };
        template <char... chs,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct duplication<t_string<'*', chs...>, NFA, Finite, Next, Start>
        {
            // X*
            using type = typename parse_regex<
                t_string<chs...>,
                fa_add_edge_t<Next,
                              value_wrapper<EMPTY_TRANS_EDGE>,
                              Start,
                              fa_add_edge_t<Start,
                                            value_wrapper<EMPTY_TRANS_EDGE>,
                                            Next,
                                            NFA>>,
                Finite,
                value_wrapper<Next::value + 1>,
                Next>::type;
        };
        template <typename Regex,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        using duplication_t =
            typename duplication<Regex, NFA, Finite, Next, Start>::type;
        template <char c,
                  char... chs,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct parse_regex<t_string<c, chs...>, NFA, Finite, Next, Start>
        {
            static constexpr char next_char = std::conditional_t<
                c == '\\',
                lazy_template<first_character_s, t_string<chs...>>,
                identity<value_wrapper<c>>>::type::value;
            using next_chs = typename std::conditional_t<
                c == '\\',
                lazy_template<ignore_one_s_t, t_string<chs...>>,
                identity<t_string<chs...>>>::type;
            using type = duplication_t<
                next_chs,
                fa_add_edge_t<Start, value_wrapper<next_char>, Next, NFA>,
                Finite,
                Next,
                Start>;
        };
        template <char... chs,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct parse_regex<t_string<'[', chs...>, NFA, Finite, Next, Start>
        {
            static_assert(sizeof...(chs) >= 1, "missing ']'");
            template <typename T>
            struct for_each_char;
            template <bool is_range, char first, typename CS>
            struct create_edge;
            template <char first, typename CS>
            struct create_edge<false, first, CS>
            {
                using type      = fa_add_edge_t<Start,
                                           value_wrapper<first>,
                                           Next,
                                           typename for_each_char<CS>::type>;
                using following = typename for_each_char<CS>::following;
            };
            template <char beg, char c, char... cs>
            struct create_edge<true, beg, t_string<'-', c, cs...>>
            {
                static_assert(sizeof...(cs) > 0, "missing ']'");
                static constexpr char next_char = std::conditional_t<
                    c == '\\',
                    lazy_template<first_character_s, t_string<cs...>>,
                    identity<value_wrapper<c>>>::type::value;
                using next_cs = typename std::conditional_t<
                    c == '\\',
                    lazy_template<ignore_one_s_t, t_string<cs...>>,
                    identity<t_string<cs...>>>::type;
                // range: beg-next_char
                using type =
                    fa_add_chars_t<Start,
                                   Next,
                                   beg,
                                   next_char + 1,
                                   typename for_each_char<next_cs>::type>;
                using following = typename for_each_char<next_cs>::following;
            };
            template <char c, char... cs>
            struct for_each_char<t_string<c, cs...>>
            {
                static constexpr char next_char = std::conditional_t<
                    c == '\\',
                    lazy_template<first_character_s, t_string<cs...>>,
                    identity<value_wrapper<c>>>::type::value;
                using next_cs = typename std::conditional_t<
                    c == '\\',
                    lazy_template<ignore_one_s_t, t_string<cs...>>,
                    identity<t_string<cs...>>>::type;
                static constexpr bool is_range =
                    t_string_size_v<next_cs> > 0 &&
                    first_character_s<next_cs>::value == '-';

                using type =
                    typename create_edge<is_range, next_char, next_cs>::type;
                using following = typename create_edge<is_range,
                                                       next_char,
                                                       next_cs>::following;
            };
            template <char... cs>
            struct for_each_char<t_string<']', cs...>>
            {
                using type      = NFA;
                using following = t_string<cs...>;
            };
            using type = duplication_t<
                typename for_each_char<t_string<chs...>>::following,
                typename for_each_char<t_string<chs...>>::type,
                Finite,
                Next,
                Start>;
        };
        template <char... chs,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct parse_regex<t_string<'.', chs...>, NFA, Finite, Next, Start>
        {
            using type = duplication_t<t_string<chs...>,
                                       fa_add_chars_t<Start, Next, 1, 128, NFA>,
                                       Finite,
                                       Next,
                                       Start>;
        };
        template <char... chs,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct parse_regex<t_string<'(', chs...>, NFA, Finite, Next, Start>
        {
            using end_Of_group = typename parse_regex<t_string<chs...>,
                                                      NFA,
                                                      Finite,
                                                      Next,
                                                      Start>::type;
            using nfa          = type_list_get<end_Of_group, 0>;
            using finite       = type_list_get<end_Of_group, 1>;
            using next         = type_list_get<end_Of_group, 2>;
            using start        = type_list_get<end_Of_group, 3>;
            using following    = type_list_get<end_Of_group, 4>;
            using type = duplication_t<following, nfa, finite, start, Start>;
        };
        template <char... chs,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        struct parse_regex<t_string<')', chs...>, NFA, Finite, Next, Start>
        {
            // end of group
            using type = type_list<NFA, Finite, Next, Start, t_string<chs...>>;
        };
        template <typename NFA, typename Finite, typename Next, typename Start>
        struct parse_regex<t_string<>, NFA, Finite, Next, Start>
        {
            using type = type_list<NFA, Finite, Next, Start>;
        };
        template <typename Regex,
                  typename NFA,
                  typename Finite,
                  typename Next,
                  typename Start>
        using parse_regex_t =
            typename parse_regex<Regex, NFA, Finite, Next, Start>::type;
    } // namespace Impl
} // namespace Compiler

#endif
