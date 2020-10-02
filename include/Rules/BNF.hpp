// ParserInString -*- C++ -*-

// Copyright (C) 2018  Scott Brown

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

#ifndef PARSER_IN_STRING_RULES_EBNF
#define PARSER_IN_STRING_RULES_EBNF

#include "Common/Identity.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "TAVL.hpp"
#include <type_traits>

namespace Compiler
{
    namespace Impl
    {
        inline namespace Keyword
        {
            // the operator and precedence defined for BNF(Backus–Naur form)
            using kw_repetition           = _TSTR("*");
            using kw_except               = _TSTR("-");
            using kw_concatenate          = _TSTR(",");
            using kw_definition_separatoe = _TSTR("|");
            using kw_defining             = _TSTR("=");
            using kw_terminator           = _TSTR(";");
            using kw_first_quote          = _TSTR("'");
            using kw_second_quote         = _TSTR("\"");
            using kw_start_comment        = _TSTR("(*");
            using kw_end_comment          = _TSTR("*)");
            using kw_start_group          = _TSTR("(");
            using kw_end_group            = _TSTR(")");
            using kw_start_optional       = _TSTR("[");
            using kw_end_optional         = _TSTR("]");
            using kw_start_repeat         = _TSTR("{");
            using kw_end_repeat           = _TSTR("}");
            using kw_special_sequence     = _TSTR("?");
            // extensions
            // predefined meta-identifier used for adding user-defined function
            using kw_do = _TSTR("?do?");
        } // namespace Keyword
#define def_single(name, ch)                                                  \
    template <typename T>                                                     \
    struct is_##name;                                                         \
    template <char C, char... chars>                                          \
    struct is_##name<t_string<C, chars...>>                                   \
    {                                                                         \
        using start                 = t_string<C, chars...>;                  \
        static constexpr bool value = (C == ch);                              \
        using next                  = std::                                   \
            conditional_t<value, t_string<chars...>, t_string<C, chars...>>;  \
    };                                                                        \
    template <>                                                               \
    struct is_##name<t_string<>>                                              \
    {                                                                         \
        using start                 = t_string<>;                             \
        static constexpr bool value = false;                                  \
        using next                  = t_string<>;                             \
    };                                                                        \
    template <char... chs>                                                    \
    inline constexpr bool is_##name##_v = is_##name<t_string<chs...>>::value; \
    template <char... chs>                                                    \
    using is_##name##_t = is_##name<t_string<chs...>>;                        \
    template <char... chs>                                                    \
    using is_##name##_n = typename is_##name##_t<chs...>::next
#define def_one_or_two(name, one, first, second)                              \
    template <typename T>                                                     \
    struct is_##name;                                                         \
    template <char... chars>                                                  \
    struct is_##name<t_string<chars...>>                                      \
    {                                                                         \
    private:                                                                  \
        template <char...>                                                    \
        struct helper;                                                        \
        template <char C1, char C2, char... chs>                              \
        struct helper<C1, C2, chs...>                                         \
        {                                                                     \
            static constexpr bool value =                                     \
                (C1 == one || (C1 == first && C2 == second));                 \
            using next =                                                      \
                std::conditional_t<value,                                     \
                                   std::conditional_t<C1 == one,              \
                                                      t_string<C2, chs...>,   \
                                                      t_string<chs...>>,      \
                                   t_string<C1, C2, chs...>>;                 \
        };                                                                    \
        template <char C1>                                                    \
        struct helper<C1>                                                     \
        {                                                                     \
            static constexpr bool value = (C1 == one);                        \
            using next = std::conditional_t<value, t_string<>, t_string<C1>>; \
        };                                                                    \
                                                                              \
    public:                                                                   \
        using start                 = t_string<chars...>;                     \
        static constexpr bool value = helper<chars...>::value;                \
        using next                  = typename helper<chars...>::next;        \
    };                                                                        \
    template <>                                                               \
    struct is_##name<t_string<>>                                              \
    {                                                                         \
        using start                 = t_string<>;                             \
        static constexpr bool value = false;                                  \
        using next                  = t_string<>;                             \
    };                                                                        \
    template <char... chs>                                                    \
    inline constexpr bool is_##name##_v = is_##name<t_string<chs...>>::value; \
    template <char... chs>                                                    \
    using is_##name##_t = is_##name<t_string<chs...>>;                        \
    template <char... chs>                                                    \
    using is_##name##_n = typename is_##name##_t<chs...>::next
        // forward declarations
        template <typename T>
        struct is_end_comment;
        template <typename T>
        struct is_start_comment;
        template <typename T>
        struct integer_symbol;
        template <typename T>
        struct meta_identifier;
        template <typename T>
        struct terminal_string;
        template <typename T>
        struct first_quote_string;
        template <typename T>
        struct second_quote_string;
        template <typename T>
        struct special_sequence;
        template <typename T>
        struct comment;
        template <typename T>
        struct rule;
        template <typename T>
        struct definitions_list;
        template <typename T>
        struct single_definition;
        template <typename T>
        struct term;
        template <typename T>
        struct optional_sequence;
        template <char... chs>
        inline constexpr bool is_optional_sequence_v =
            optional_sequence<t_string<chs...>>::value;
        template <char... chs>
        using optional_sequence_t = optional_sequence<t_string<chs...>>;
        template <char... chs>
        using optional_sequence_v =
            typename optional_sequence_t<chs...>::symbol;
        template <char... chs>
        using optional_sequence_n = typename optional_sequence_t<chs...>::next;
        template <typename T>
        struct repeated_sequence;
        template <char... chs>
        inline constexpr bool is_repeated_sequence_v =
            repeated_sequence<t_string<chs...>>::value;
        template <char... chs>
        using repeated_sequence_t = repeated_sequence<t_string<chs...>>;
        template <char... chs>
        using repeated_sequence_v =
            typename repeated_sequence_t<chs...>::symbol;
        template <char... chs>
        using repeated_sequence_n = typename repeated_sequence_t<chs...>::next;
        template <typename T>
        struct grouped_sequence;
        template <char... chs>
        inline constexpr bool is_grouped_sequence_v =
            grouped_sequence<t_string<chs...>>::value;
        template <char... chs>
        using grouped_sequence_t = grouped_sequence<t_string<chs...>>;
        template <char... chs>
        using grouped_sequence_v = typename grouped_sequence_t<chs...>::symbol;
        template <char... chs>
        using grouped_sequence_n = typename grouped_sequence_t<chs...>::next;
        template <typename T>
        struct empty_sequence;
        template <typename T>
        struct remove_leading_space_and_comment;
        // letter
        template <char ch>
        inline constexpr bool is_alpha_v = ('a' <= ch && ch <= 'z') ||
                                           ('A' <= ch && ch <= 'Z');
        // decimal digit
        template <char ch>
        inline constexpr bool is_digit_v = ('0' <= ch && ch <= '9');
        // concatenate symbol
        def_single(concatenate, ',');
        // defining symbol
        def_single(defining, '=');
        // definition separator symbol
        template <typename T>
        struct is_definition_separator;
        template <char... chars>
        struct is_definition_separator<t_string<chars...>>
        {
        private:
            template <char...>
            struct helper;
            template <char C1, char C2, char... chs>
            struct helper<C1, C2, chs...>
            {
                static constexpr bool value =
                    (C1 == '|' || C1 == '!' || (C1 == '/' && C2 != ')'));
                using next = std::conditional_t<value,
                                                t_string<C2, chs...>,
                                                t_string<C1, C2, chs...>>;
            };
            template <char C1>
            struct helper<C1>
            {
                static constexpr bool value =
                    (C1 == '|' || C1 == '/' || C1 == '!');
                using next =
                    std::conditional_t<value, t_string<>, t_string<C1>>;
            };

        public:
            using start                 = t_string<chars...>;
            static constexpr bool value = helper<chars...>::value;
            using next                  = typename helper<chars...>::next;
        };
        template <>
        struct is_definition_separator<t_string<>>
        {
            using start                 = t_string<>;
            static constexpr bool value = false;
            using next                  = t_string<>;
        };
        template <char... chs>
        inline constexpr bool is_definition_separator_v =
            is_definition_separator<t_string<chs...>>::value;
        template <char... chs>
        using is_definition_separator_t =
            is_definition_separator<t_string<chs...>>;
        template <char... chs>
        using is_definition_separator_n =
            typename is_definition_separator_t<chs...>::next;
        // end comment symbol
        template <char... chars>
        struct is_end_comment<t_string<chars...>>
        {
        private:
            template <char...>
            struct helper;
            template <char C1, char C2, char... chs>
            struct helper<C1, C2, chs...>
            {
                static constexpr bool value = (C1 == '*' && C2 == ')');
                using next                  = std::conditional_t<value,
                                                t_string<chs...>,
                                                t_string<C1, C2, chs...>>;
            };
            template <char C1>
            struct helper<C1>
            {
                static constexpr bool value = false;
                using next                  = t_string<C1>;
            };

        public:
            using start                 = t_string<chars...>;
            static constexpr bool value = helper<chars...>::value;
            using next                  = typename helper<chars...>::next;
        };
        template <>
        struct is_end_comment<t_string<>>
        {
            using start                 = t_string<>;
            static constexpr bool value = false;
            using next                  = t_string<>;
        };
        template <char... chs>
        inline constexpr bool is_end_comment_v =
            is_end_comment<t_string<chs...>>::value;
        template <char... chs>
        using is_end_comment_t = is_end_comment<t_string<chs...>>;
        template <char... chs>
        using is_end_comment_n = typename is_end_comment_t<chs...>::next;
        // end group symbol
        def_single(end_group, ')');
        // end option symbol
        def_one_or_two(end_option, ']', '/', ')');
        // end repeat symbol
        def_one_or_two(end_repeat, '}', ':', ')');
        // except symbol
        def_single(except, '-');
        // first quote symbol
        def_single(first_quote, '\'');
        // repetition symbol
        def_single(repetition, '*');
        // second quote symbol
        def_single(second_quote, '"');
        // special sequence symbol
        def_single(special_sequence_symbol, '?');
        // space
        template <typename T>
        struct is_space;
        template <char C, char... chars>
        struct is_space<t_string<C, chars...>>
        {
        private:
            template <typename T>
            struct next_wrapper
            {
                using next = T;
            };

        public:
            using start                 = t_string<C, chars...>;
            static constexpr bool value = (C == ' ') || (C == '\t') ||
                                          (C == '\v') || (C == '\f') ||
                                          (C == '\n');
            using next = typename std::conditional_t<
                value,
                tavl::lazy_template<is_space, t_string<chars...>>,
                tavl::lazy_template<next_wrapper,
                                    t_string<C, chars...>>>::type::next;
        };
        template <>
        struct is_space<t_string<>>
        {
            using start                 = t_string<>;
            static constexpr bool value = false;
            using next                  = t_string<>;
        };
        template <char... chs>
        inline constexpr bool is_space_v = is_space<t_string<chs...>>::value;
        template <char... chs>
        using is_space_t = is_space<t_string<chs...>>;
        template <char... chs>
        using is_space_n = typename is_space_t<chs...>::next;
        // start comment symbol
        template <char... chars>
        struct is_start_comment<t_string<chars...>>
        {
        private:
            template <char...>
            struct helper;
            template <char C1, char C2, char... chs>
            struct helper<C1, C2, chs...>
            {
                static constexpr bool value = (C1 == '(' && C2 == '*');
                using next                  = std::conditional_t<value,
                                                t_string<chs...>,
                                                t_string<C1, C2, chs...>>;
            };
            template <char C1>
            struct helper<C1>
            {
                static constexpr bool value = false;
                using next                  = t_string<C1>;
            };

        public:
            using start                 = t_string<chars...>;
            static constexpr bool value = helper<chars...>::value;
            using next                  = typename helper<chars...>::next;
        };
        template <>
        struct is_start_comment<t_string<>>
        {
            using start                 = t_string<>;
            static constexpr bool value = false;
            using next                  = t_string<>;
        };
        template <char... chs>
        inline constexpr bool is_start_comment_v =
            is_start_comment<t_string<chs...>>::value;
        template <char... chs>
        using is_start_comment_t = is_start_comment<t_string<chs...>>;
        template <char... chs>
        using is_start_comment_n = typename is_start_comment_t<chs...>::next;
        // start group symbol
        def_single(start_group, '(');
        // start option symbol
        def_one_or_two(start_option, '[', '(', '/');
        // start repeat symbol
        def_one_or_two(start_repeat, '{', '(', ':');
        // terminator symbol
        def_single(terminator, ';' || C == '.');
        // other character
        // ' ' | ':' | '+' | '_' | '%' | '@'
        // | '&' | '#' | '$' | '<' | '>' | '\'
        // | '^' | '`' | '~'
        // integer
        template <char... chars>
        struct integer_symbol<t_string<chars...>>
        {
        private:
            template <char D, typename T>
            struct integer_maker
            {
                static constexpr unsigned int base = T::base * 10;
                using type = std::integral_constant<unsigned int,
                                                    (D - '0') * T::base +
                                                        T::type::value>;
            };
            template <char Z>
            struct integer_maker<Z, t_string<>>
            {
                using type = std::integral_constant<unsigned int, 0>;
                static constexpr unsigned int base = 1;
            };
            template <char...>
            struct helper;
            template <char C, char... chs>
            struct helper<C, chs...>
            {
                template <char... cs>
                struct lazy:
                    public identity<
                        integer_maker<C, typename helper<cs...>::symbol>>
                {};
                template <char... cs>
                struct lazy_n: public identity<typename helper<cs...>::next>
                {};
                static constexpr bool value = is_digit_v<C>;
                using symbol                = typename std::conditional_t<
                    is_digit_v<C>,
                    lazy<chs...>,
                    identity<integer_maker<'\0', t_string<>>>>::type;
                using next = typename std::conditional_t<
                    is_digit_v<C>,
                    lazy_n<chs...>,
                    identity<t_string<C, chs...>>>::type;
            };

        public:
            using start                         = t_string<chars...>;
            static constexpr bool         match = helper<chars...>::value;
            static constexpr unsigned int value =
                helper<chars...>::symbol::type::value;
            using next = typename helper<chars...>::next;
        };
        template <>
        struct integer_symbol<t_string<>>
        {
            using start                         = t_string<>;
            static constexpr bool         match = false;
            static constexpr unsigned int value = 0;
            using next                          = t_string<>;
        };
        template <char... chs>
        inline constexpr bool is_integer_symbol_v =
            integer_symbol<t_string<chs...>>::match;
        template <char... chs>
        inline constexpr unsigned int integer_symbol_v =
            integer_symbol<t_string<chs...>>::value;
        template <char... chs>
        using integer_symbol_t = integer_symbol<t_string<chs...>>;
        template <char... chs>
        using integer_symbol_n = typename integer_symbol_t<chs...>::next;
        // meta identifier
        template <char ch>
        inline constexpr bool is_meta_identifier_character_v = is_alpha_v<ch> ||
                                                               is_digit_v<ch> ||
                                                               (ch == ' ') ||
                                                               (ch == '\t') ||
                                                               (ch == '\v') ||
                                                               (ch == '\f');
        template <char... chars>
        struct meta_identifier<t_string<chars...>>
        {
        private:
            template <char... chs>
            struct helper;
            template <char C, char... chs>
            struct helper<C, chs...>
            {
                template <char... cs>
                struct lazy_s:
                    public identity<decltype(
                        concat(t_string<C>(),
                               typename helper<chs...>::symbol()))>
                {};
                template <char... cs>
                struct lazy_n: public identity<typename helper<chs...>::next>
                {};
                using symbol = typename std::conditional_t<
                    is_meta_identifier_character_v<C>,
                    lazy_s<chs...>,
                    identity<t_string<>>>::type;
                using next = typename std::conditional_t<
                    is_meta_identifier_character_v<C>,
                    lazy_n<chs...>,
                    identity<t_string<C, chs...>>>::type;
            };
            template <char C>
            struct helper<C>
            {
                using symbol =
                    std::conditional_t<is_meta_identifier_character_v<C>,
                                       t_string<C>,
                                       t_string<>>;
                using next =
                    std::conditional_t<is_meta_identifier_character_v<C>,
                                       t_string<>,
                                       t_string<C>>;
            };

        public:
            using start  = t_string<chars...>;
            using symbol = std::conditional_t<
                is_alpha_v<first_character_v<chars...>>,
                merge_space<typename helper<chars...>::symbol>,
                t_string<>>;
            using next                  = typename helper<chars...>::next;
            static constexpr bool value = !std::is_same_v<symbol, t_string<>>;
        };
        template <>
        struct meta_identifier<t_string<>>
        {
            using start                 = t_string<>;
            using symbol                = t_string<>;
            using next                  = t_string<>;
            static constexpr bool value = false;
        };
        template <char... chs>
        inline constexpr bool is_meta_identifier_v =
            meta_identifier<t_string<chs...>>::value;
        template <char... chs>
        using meta_identifier_v =
            typename meta_identifier<t_string<chs...>>::symbol;
        template <char... chs>
        using meta_identifier_t = meta_identifier<t_string<chs...>>;
        template <char... chs>
        using meta_identifier_n =
            typename meta_identifier<t_string<chs...>>::next;
        // first quote string
        template <char... chars>
        struct first_quote_string<t_string<chars...>>
        {
        private:
            struct missing_first_quote;
            template <char... chs>
            struct helper;
            template <char C, char... chs>
            struct helper<C, chs...>
            {
                template <char... cs>
                struct lazy_s:
                    public identity<decltype(
                        concat(t_string<C>(),
                               std::declval<typename helper<cs...>::symbol>()))>
                {};
                template <char... cs>
                struct lazy_n: public identity<typename helper<cs...>::next>
                {};
                using symbol =
                    typename std::conditional_t<!is_first_quote_v<C>,
                                                lazy_s<chs...>,
                                                identity<t_string<>>>::type;
                using next = typename std::conditional_t<
                    !is_first_quote_v<C>,
                    lazy_n<chs...>,
                    identity<t_string<chs...>>>::type;
            };
            template <char C>
            struct helper<C>
            {
                using symbol = std::conditional_t<!is_first_quote_v<C>,
                                                  missing_first_quote,
                                                  t_string<>>;
                using next   = std::conditional_t<!is_first_quote_v<C>,
                                                missing_first_quote,
                                                t_string<>>;
            };
            template <char... chs>
            struct helper_with_start
            {
                template <typename T>
                struct unpack_s;
                template <char... cs>
                struct unpack_s<t_string<cs...>>:
                    public identity<typename helper<cs...>::symbol>
                {};
                template <typename T>
                struct unpack_n;
                template <char... cs>
                struct unpack_n<t_string<cs...>>:
                    public identity<typename helper<cs...>::next>
                {};
                using symbol =
                    typename std::conditional_t<is_first_quote_v<chs...>,
                                                unpack_s<ignore_one_t<chs...>>,
                                                identity<t_string<>>>::type;
                using next = typename std::conditional_t<
                    is_first_quote_v<chs...>,
                    unpack_n<ignore_one_t<chs...>>,
                    identity<t_string<chs...>>>::type;
            };
            template <typename T>
            struct has_close_quote;
            template <char... chs>
            struct has_close_quote<t_string<chs...>>
            {
                static constexpr bool value = (is_first_quote_v<chs> || ...);
            };

        public:
            using start  = t_string<chars...>;
            using symbol = typename helper_with_start<chars...>::symbol;
            using next   = typename helper_with_start<chars...>::next;
            // according to ISO/IEC 14977:1996, a first-quote string should have
            // at least one character.
            // see ISO/IEC 14977:1996 $4.16
            static constexpr bool value =
                is_first_quote_v<chars...> &&
                !is_first_quote<is_first_quote_n<chars...>>::value &&
                has_close_quote<is_first_quote_n<chars...>>::value;
        };
        template <>
        struct first_quote_string<t_string<>>
        {
            using start                 = t_string<>;
            using symbol                = t_string<>;
            using next                  = t_string<>;
            static constexpr bool value = false;
        };
        template <char... chs>
        inline constexpr bool is_first_quote_string_v =
            first_quote_string<t_string<chs...>>::value;
        template <char... chs>
        using first_quote_string_t = first_quote_string<t_string<chs...>>;
        template <char... chs>
        using first_quote_string_v =
            typename first_quote_string_t<chs...>::symbol;
        template <char... chs>
        using first_quote_string_n =
            typename first_quote_string_t<chs...>::next;
        // second quote string
        template <char... chars>
        struct second_quote_string<t_string<chars...>>
        {
        private:
            struct missing_second_quote;
            template <char... chs>
            struct helper;
            template <char C, char... chs>
            struct helper<C, chs...>
            {
                template <char... cs>
                struct lazy_s:
                    public identity<decltype(
                        concat(t_string<C>(),
                               std::declval<typename helper<cs...>::symbol>()))>
                {};
                template <char... cs>
                struct lazy_n: public identity<typename helper<cs...>::next>
                {};
                using symbol =
                    typename std::conditional_t<!is_second_quote_v<C>,
                                                lazy_s<chs...>,
                                                identity<t_string<>>>::type;
                using next = typename std::conditional_t<
                    !is_second_quote_v<C>,
                    lazy_n<chs...>,
                    identity<t_string<chs...>>>::type;
            };
            template <char C>
            struct helper<C>
            {
                using symbol = std::conditional_t<!is_second_quote_v<C>,
                                                  missing_second_quote,
                                                  t_string<>>;
                using next   = std::conditional_t<!is_second_quote_v<C>,
                                                missing_second_quote,
                                                t_string<>>;
            };
            template <char... chs>
            struct helper_with_start
            {
                template <typename T>
                struct unpack_s;
                template <char... cs>
                struct unpack_s<t_string<cs...>>:
                    public identity<typename helper<cs...>::symbol>
                {};
                template <typename T>
                struct unpack_n;
                template <char... cs>
                struct unpack_n<t_string<cs...>>:
                    public identity<typename helper<cs...>::next>
                {};
                using symbol =
                    typename std::conditional_t<is_second_quote_v<chs...>,
                                                unpack_s<ignore_one_t<chs...>>,
                                                identity<t_string<>>>::type;
                using next = typename std::conditional_t<
                    is_second_quote_v<chs...>,
                    unpack_n<ignore_one_t<chs...>>,
                    identity<t_string<chs...>>>::type;
            };
            template <typename T>
            struct has_close_quote;
            template <char... chs>
            struct has_close_quote<t_string<chs...>>
            {
                static constexpr bool value = (is_second_quote_v<chs> || ...);
            };

        public:
            using start  = t_string<chars...>;
            using symbol = typename helper_with_start<chars...>::symbol;
            using next   = typename helper_with_start<chars...>::next;
            // according to ISO/IEC 14977:1996, a second quote string should
            // have at least one character. see ISO/IEC 14977:1996 $4.16
            static constexpr bool value =
                is_second_quote_v<chars...> &&
                !is_second_quote<is_second_quote_n<chars...>>::value &&
                has_close_quote<is_second_quote_n<chars...>>::value;
        };
        template <>
        struct second_quote_string<t_string<>>
        {
            using start                 = t_string<>;
            using symbol                = t_string<>;
            using next                  = t_string<>;
            static constexpr bool value = false;
        };
        template <char... chs>
        inline constexpr bool is_second_quote_string_v =
            second_quote_string<t_string<chs...>>::value;
        template <char... chs>
        using second_quote_string_t = second_quote_string<t_string<chs...>>;
        template <char... chs>
        using second_quote_string_v =
            typename second_quote_string_t<chs...>::symbol;
        template <char... chs>
        using second_quote_string_n =
            typename second_quote_string_t<chs...>::next;
        // terminal string
        template <char... chars>
        struct terminal_string<t_string<chars...>>
        {
        private:
            template <char... chs>
            struct lazy_first_s: public identity<first_quote_string_v<chs...>>
            {};
            template <char... chs>
            struct lazy_second_s: public identity<second_quote_string_v<chs...>>
            {};
            template <char... chs>
            struct lazy_first_n: public identity<first_quote_string_n<chs...>>
            {};
            template <char... chs>
            struct lazy_second_n: public identity<second_quote_string_n<chs...>>
            {};

        public:
            using start  = t_string<chars...>;
            using symbol = typename std::conditional_t<
                is_first_quote_v<chars...>,
                lazy_first_s<chars...>,
                std::conditional_t<is_second_quote_v<chars...>,
                                   lazy_second_s<chars...>,
                                   identity<t_string<>>>>::type;
            using next = typename std::conditional_t<
                is_first_quote_v<chars...>,
                lazy_first_n<chars...>,
                std::conditional_t<is_second_quote_v<chars...>,
                                   lazy_second_n<chars...>,
                                   identity<t_string<chars...>>>>::type;
            static constexpr bool value = (is_first_quote_string_v<chars...> ||
                                           is_second_quote_string_v<chars...>);
        };
        template <>
        struct terminal_string<t_string<>>
        {
            using start                 = t_string<>;
            using symbol                = t_string<>;
            using next                  = t_string<>;
            static constexpr bool value = false;
        };
        template <char... chs>
        inline constexpr bool is_terminal_string_v =
            terminal_string<t_string<chs...>>::value;
        template <char... chs>
        using terminal_string_t = terminal_string<t_string<chs...>>;
        template <char... chs>
        using terminal_string_v = typename terminal_string_t<chs...>::symbol;
        template <char... chs>
        using terminal_string_n = typename terminal_string_t<chs...>::next;
        // special sequence
        template <char... chars>
        struct special_sequence<t_string<chars...>>
        {
        private:
            struct missing_space_sequence_symbol;
            template <char... chs>
            struct helper;
            template <char C, char... chs>
            struct helper<C, chs...>
            {
                template <char... cs>
                struct lazy_s:
                    public identity<decltype(
                        concat(t_string<C>(),
                               typename helper<cs...>::symbol()))>
                {};
                template <char... cs>
                struct lazy_n: public identity<typename helper<cs...>::next>
                {};
                using symbol = typename std::conditional_t<
                    !is_special_sequence_symbol_v<C>,
                    lazy_s<chs...>,
                    identity<t_string<>>>::type;
                using next = typename std::conditional_t<
                    !is_special_sequence_symbol_v<C>,
                    lazy_n<chs...>,
                    identity<t_string<chs...>>>::type;
            };
            template <char C>
            struct helper<C>
            {
                using symbol =
                    std::conditional_t<!is_special_sequence_symbol_v<C>,
                                       missing_space_sequence_symbol,
                                       t_string<>>;
                using next =
                    std::conditional_t<!is_special_sequence_symbol_v<C>,
                                       missing_space_sequence_symbol,
                                       t_string<>>;
            };
            template <char... chs>
            struct helper_with_start
            {
                template <typename T>
                struct unpack_s;
                template <char... cs>
                struct unpack_s<t_string<cs...>>:
                    public identity<typename helper<cs...>::symbol>
                {};
                template <typename T>
                struct unpack_n;
                template <char... cs>
                struct unpack_n<t_string<cs...>>:
                    public identity<typename helper<cs...>::next>
                {};
                using symbol = typename std::conditional_t<
                    is_special_sequence_symbol_v<chs...>,
                    unpack_s<ignore_one_t<chs...>>,
                    identity<t_string<>>>::type;
                using next = typename std::conditional_t<
                    is_special_sequence_symbol_v<chs...>,
                    unpack_n<ignore_one_t<chs...>>,
                    identity<t_string<chs...>>>::type;
            };
            template <typename T>
            struct has_close_quote;
            template <char... chs>
            struct has_close_quote<t_string<chs...>>
            {
                static constexpr bool value =
                    (... || is_special_sequence_symbol_v<chs>);
            };

        public:
            using start  = t_string<chars...>;
            using symbol = typename helper_with_start<chars...>::symbol;
            using next   = typename helper_with_start<chars...>::next;
            // according to ISO/IEC 14977:1996, a second quote string should
            // have at least one character. see ISO/IEC 14977:1996 $4.16
            static constexpr bool value =
                is_special_sequence_symbol_v<chars...> &&
                !is_second_quote<
                    is_special_sequence_symbol_n<chars...>>::value &&
                has_close_quote<is_special_sequence_symbol_n<chars...>>::value;
        };
        template <>
        struct special_sequence<t_string<>>
        {
            using start                 = t_string<>;
            using symbol                = t_string<>;
            using next                  = t_string<>;
            static constexpr bool value = false;
        };
        template <char... chs>
        inline constexpr bool is_special_sequence_v =
            special_sequence<t_string<chs...>>::value;
        template <char... chs>
        using special_sequence_t = special_sequence<t_string<chs...>>;
        template <char... chs>
        using special_sequence_v = typename special_sequence_t<chs...>::symbol;
        template <char... chs>
        using special_sequence_n = typename special_sequence_t<chs...>::next;
        // comment
        // note: We use the informal definition of 'comment' defined in
        // ISO/IEC 14977:1996 $8.2
        // Nested comments are allowed.
        // Therefore every (*  should have a corresponding *) to match it.
        template <char... chars>
        struct comment<t_string<chars...>>
        {
        private:
            struct not_match
            {
                using symbol = not_match;
                using next   = not_match;
            };
            template <char...>
            struct comment_symbol;
            template <typename T>
            struct bracketed_textual_inner
            {
                static_assert(std::is_same_v<T, t_string<>>);
                using symbol                = t_string<>;
                using next                  = t_string<>;
                using value_t               = std::false_type;
                static constexpr bool value = false;
            };
            template <char... chs>
            struct bracketed_textual
            {
                template <char... cs>
                struct lazy:
                    public identity<
                        bracketed_textual_inner<is_start_comment_n<cs...>>>
                {};
                struct next_wrapper
                {
                    using next = t_string<chs...>;
                };
                static constexpr bool is_valid = is_start_comment_v<chs...>;
                using symbol                   = typename std::conditional_t<
                    is_valid,
                    tavl::lazy_template<bracketed_textual_inner,
                                        is_start_comment_n<chs...>>,
                    identity<not_match>>::type::symbol;
                using next = typename std::conditional_t<
                    is_valid,
                    tavl::lazy_template<bracketed_textual_inner,
                                        is_start_comment_n<chs...>>,
                    identity<next_wrapper>>::type::next;
                static constexpr bool value = std::conditional_t<
                    is_valid,
                    tavl::lazy_template<bracketed_textual_inner,
                                        is_start_comment_n<chs...>>,
                    identity<std::false_type>>::type::value;
                // is_valid && lazy<chs...>::type::value;
            };
            template <char... chs>
            struct bracketed_textual_inner<t_string<chs...>>
            {
                template <char... cs>
                struct recursive_match_s:
                    public identity<decltype(concat(
                        std::declval<typename comment_symbol<cs...>::symbol>(),
                        std::declval<typename bracketed_textual_inner<
                            typename comment_symbol<cs...>::next>::symbol>()))>
                {};
                template <char... cs>
                struct recursive_match_n:
                    public identity<typename bracketed_textual_inner<
                        typename comment_symbol<cs...>::next>::next>
                {};
                template <char... cs>
                struct recursive_match_v:
                    public identity<typename bracketed_textual_inner<
                        typename comment_symbol<cs...>::next>::value_t>
                {};
                static constexpr bool is_end = comment_symbol<chs...>::end;
                using symbol                 = typename std::conditional_t<
                    is_end,
                    identity<t_string<>>,
                    recursive_match_s<chs...>>::type;
                using next = typename std::conditional_t<
                    is_end,
                    identity<is_end_comment_n<chs...>>,
                    recursive_match_n<chs...>>::type;
                using value_t = typename std::conditional_t<
                    is_end,
                    identity<std::true_type>,
                    recursive_match_v<chs...>>::type;
                static constexpr bool value = value_t::value;
            };
            template <char C>
            struct bracketed_textual_inner<t_string<C>>
            {
                using symbol                = t_string<>;
                using next                  = t_string<C>;
                using value_t               = std::false_type;
                static constexpr bool value = false;
            };
            template <char... chs>
            struct comment_symbol
            {
                template <char... cs>
                struct lazy_bracketed_s:
                    public identity<typename bracketed_textual<cs...>::symbol>
                {};
                template <char... cs>
                struct lazy_terminal_s:
                    public identity<terminal_string_v<cs...>>
                {};
                template <char... cs>
                struct lazy_character_s:
                    public identity<first_character_t<cs...>>
                {};
                template <char...>
                struct lazy_end_s: public identity<t_string<>>
                {};
                template <char... cs>
                struct lazy_bracketed_n:
                    public identity<typename bracketed_textual<cs...>::next>
                {};
                template <char... cs>
                struct lazy_terminal_n:
                    public identity<terminal_string_n<cs...>>
                {};
                template <char... cs>
                struct lazy_character_n: public identity<ignore_one_t<cs...>>
                {};
                template <char... cs>
                struct lazy_end_n: public identity<is_end_comment_n<cs...>>
                {};
                static constexpr bool end = is_end_comment_v<chs...>;
                using symbol              = typename std::conditional_t<
                    is_end_comment_v<chs...>,
                    lazy_end_s<chs...>,
                    std::conditional_t<
                        is_start_comment_v<chs...>,
                        lazy_bracketed_s<chs...>,
                        std::conditional_t<is_terminal_string_v<chs...>,
                                           lazy_terminal_s<chs...>,
                                           lazy_character_s<chs...>>>>::type;
                using next = typename std::conditional_t<
                    is_end_comment_v<chs...>,
                    lazy_end_n<chs...>,
                    std::conditional_t<
                        is_start_comment_v<chs...>,
                        lazy_bracketed_n<chs...>,
                        std::conditional_t<is_terminal_string_v<chs...>,
                                           lazy_terminal_n<chs...>,
                                           lazy_character_n<chs...>>>>::type;
            };

        public:
            using start  = t_string<chars...>;
            using symbol = typename bracketed_textual<chars...>::symbol;
            using next   = typename bracketed_textual<chars...>::next;
            static constexpr bool value = bracketed_textual<chars...>::value;
        };
        template <char C>
        struct comment<t_string<C>>
        {
            using start                 = t_string<C>;
            using symbol                = t_string<>;
            using next                  = t_string<C>;
            static constexpr bool value = false;
        };
        template <>
        struct comment<t_string<>>
        {
            using start                 = t_string<>;
            using symbol                = t_string<>;
            using next                  = t_string<>;
            static constexpr bool value = false;
        };
        template <char... chs>
        inline constexpr bool is_comment_v = comment<t_string<chs...>>::value;
        template <char... chs>
        using comment_t = comment<t_string<chs...>>;
        template <char... chs>
        using comment_v = typename comment_t<chs...>::symbol;
        template <char... chs>
        using comment_n = typename comment_t<chs...>::next;
        template <char... chars>
        struct remove_leading_space_and_comment<t_string<chars...>>
        {
        private:
            template <typename T>
            struct helper;
            template <char... chs>
            struct helper<t_string<chs...>>:
                std::conditional_t<
                    is_space_v<chs...>,
                    helper<is_space_n<chs...>>,
                    std::conditional_t<is_comment_v<chs...>,
                                       helper<comment_n<chs...>>,
                                       identity<t_string<chs...>>>>
            {};

        public:
            using start = t_string<chars...>;
            using next  = typename helper<t_string<chars...>>::type;
            static constexpr bool value =
                is_space_v<chars...> || is_comment_v<chars...>;
        };
        template <char... chs>
        using remove_leading_space_and_comment_n =
            typename remove_leading_space_and_comment<t_string<chs...>>::next;
#undef def_one_or_two
#undef def_single
        struct optional_sequence_flag
        {};
        struct repeated_sequence_flag
        {};
        struct grouped_sequence_flag
        {};
        struct meta_identifier_flag
        {};
        struct terminal_string_flag
        {};
        struct special_sequence_flag
        {};
        struct empty_sequence_flag
        {};
#define start_def_end(name, ttype)                                             \
    template <char... chars>                                                   \
    struct name<t_string<chars...>>                                            \
    {                                                                          \
    private:                                                                   \
        template <typename T>                                                  \
        struct definitions_invalid_helper                                      \
        {                                                                      \
            using symbol                = void;                                \
            using next                  = t_string<>;                          \
            static constexpr bool value = false;                               \
        };                                                                     \
        using definitions = std::conditional_t<                                \
            is_start_##ttype##_v<chars...>,                                    \
            tavl::lazy_template<                                               \
                definitions_list,                                              \
                typename is_start_##ttype<                                     \
                    remove_leading_space_and_comment_n<chars...>>::next>,      \
            tavl::lazy_template<definitions_invalid_helper, void>>;            \
        static constexpr bool definitions_value =                              \
            std::conditional_t<is_start_##ttype##_v<chars...>,                 \
                               std::bool_constant<definitions::type::value>,   \
                               std::bool_constant<false>>::value;              \
                                                                               \
    public:                                                                    \
        static constexpr bool value =                                          \
            is_start_##ttype##_v<chars...> && definitions_value &&             \
            is_end_##ttype<typename definitions::type::next>::value;           \
        using flag   = name##_flag;                                            \
        using start  = t_string<chars...>;                                     \
        using symbol = std::conditional_t<                                     \
            value,                                                             \
            type_pair<flag, typename definitions::type::symbol>,               \
            void>;                                                             \
        using next =                                                           \
            typename remove_leading_space_and_comment<typename is_end_##ttype< \
                typename definitions::type::next>::next>::next;                \
    }
        start_def_end(optional_sequence, option);
        start_def_end(repeated_sequence, repeat);
        start_def_end(grouped_sequence, group);
#undef start_def_end
#define def_wrapper(name)                                               \
    template <typename T>                                               \
    struct name##_wrapper                                               \
    {};                                                                 \
    template <char... chars>                                            \
    struct name##_wrapper<t_string<chars...>>                           \
    {                                                                   \
    public:                                                             \
        using flag   = name##_flag;                                     \
        using start  = t_string<chars...>;                              \
        using symbol = type_pair<flag, name##_v<chars...>>;             \
        using next   = typename remove_leading_space_and_comment<       \
            name##_n<chars...>>::next;                                \
        static constexpr bool value = is_##name##_v<chars...>;          \
    };                                                                  \
    template <char... chs>                                              \
    inline constexpr bool is_##name##_wrapper_v =                       \
        name<t_string<chs...>>::value;                                  \
    template <char... chs>                                              \
    using name##_wrapper_t = name##_wrapper<t_string<chs...>>;          \
    template <char... chs>                                              \
    using name##_wrapper_v = typename name##_wrapper_t<chs...>::symbol; \
    template <char... chs>                                              \
    using name##_wrapper_n = typename name##_wrapper_t<chs...>::next
        def_wrapper(meta_identifier);
        def_wrapper(terminal_string);
        def_wrapper(special_sequence);
#undef def_wrapper
        template <char... chars>
        struct empty_sequence<t_string<chars...>>
        {
            using flag   = empty_sequence_flag;
            using start  = t_string<chars...>;
            using symbol = type_pair<flag, t_string<>>;
            using next   = remove_leading_space_and_comment_n<chars...>;
            static constexpr bool value = true;
        };
        template <char... chars>
        using empty_sequence_t = empty_sequence<t_string<chars...>>;
        template <char... chars>
        using empty_sequence_v = typename empty_sequence_t<chars...>::symbol;
        template <char... chars>
        using empty_sequence_n = typename empty_sequence_t<chars...>::next;
        template <char... chars>
        struct term<t_string<chars...>>
        {
        private:
#define def_helper(name, cond, val, next)                             \
    template <typename rhs, char... chs>                              \
    struct name##_helper:                                             \
        std::conditional_t<cond<chs...>, identity<val<chs...>>, rhs>  \
    {};                                                               \
    template <typename rhs, char... chs>                              \
    struct name##_helper_n:                                           \
        std::conditional_t<cond<chs...>, identity<next<chs...>>, rhs> \
    {}
            def_helper(optional,
                       is_optional_sequence_v,
                       optional_sequence_v,
                       optional_sequence_n);
            def_helper(repeated,
                       is_repeated_sequence_v,
                       repeated_sequence_v,
                       repeated_sequence_n);
            def_helper(grouped,
                       is_grouped_sequence_v,
                       grouped_sequence_v,
                       grouped_sequence_n);
            def_helper(meta,
                       is_meta_identifier_wrapper_v,
                       meta_identifier_wrapper_v,
                       meta_identifier_wrapper_n);
            def_helper(terminal_string,
                       is_terminal_string_wrapper_v,
                       terminal_string_wrapper_v,
                       terminal_string_wrapper_n);
            def_helper(special,
                       is_special_sequence_wrapper_v,
                       special_sequence_wrapper_v,
                       special_sequence_wrapper_n);
#undef def_helper
            template <typename T>
            struct primary_helper;
            template <char... chs>
            struct primary_helper<t_string<chs...>>:
                optional_helper<
                    repeated_helper<
                        grouped_helper<
                            meta_helper<
                                terminal_string_helper<
                                    special_helper<
                                        identity<empty_sequence_v<chs...>>,
                                        chs...>,
                                    chs...>,
                                chs...>,
                            chs...>,
                        chs...>,
                    chs...>
            {};
            template <typename T>
            struct next_helper;
            template <char... chs>
            struct next_helper<t_string<chs...>>:
                optional_helper_n<
                    repeated_helper_n<
                        grouped_helper_n<
                            meta_helper_n<
                                terminal_string_helper_n<
                                    special_helper_n<identity<t_string<chs...>>,
                                                     chs...>,
                                    chs...>,
                                chs...>,
                            chs...>,
                        chs...>,
                    chs...>
            {};
            // factor
            static constexpr bool start_with_integer =
                is_integer_symbol_v<chars...> &&
                is_repetition<typename remove_leading_space_and_comment<
                    integer_symbol_n<chars...>>::next>::value;
            static constexpr unsigned int leading_integer =
                (start_with_integer ? integer_symbol_v<chars...> : 0);
            // primary
            using primary_start = typename std::conditional_t<
                start_with_integer,
                identity<typename is_repetition<
                    typename remove_leading_space_and_comment<
                        integer_symbol_n<chars...>>::next>::next>,
                identity<t_string<chars...>>>::type;
            using primary =
                typename primary_helper<primary_start>::type; //! pair<flag,
                                                              //! value>
            using primary_next = typename next_helper<primary_start>::type;
            using primary_after_repeat = typename std::conditional_t<
                start_with_integer,
                identity<type_list_push_back_n<type_list<>,
                                               primary,
                                               leading_integer>>,
                identity<type_list<primary>>>::type;

        public:
            using flag   = typename primary::first_type;
            using start  = t_string<chars...>;
            using symbol = primary_after_repeat; // type_list
            using next =
                typename remove_leading_space_and_comment<primary_next>::next;

            static constexpr unsigned int repetition = leading_integer;
        };
        template <char... chs>
        using term_t = term<t_string<chs...>>;
        template <char... chs>
        using term_n = typename term_t<chs...>::next;
        template <char... chs>
        using term_v = typename term_t<chs...>::symbol;
        template <char... chars>
        struct single_definition<t_string<chars...>>
        {
        private:
            using leading_term =
                term<remove_leading_space_and_comment_n<chars...>>; // type_list
            using recursion_start = typename leading_term::next;
            template <typename T, typename Result>
            struct term_list_helper;
            template <typename Result, char C, char... chs>
            struct term_list_helper<t_string<C, chs...>, Result>:
                std::conditional_t<
                    is_concatenate_v<C>,
                    identity<typename term_list_helper<
                        typename remove_leading_space_and_comment<typename term<
                            remove_leading_space_and_comment_n<chs...>>::next>::
                            next,
                        type_list_push_back_m<
                            Result,
                            typename term<remove_leading_space_and_comment_n<
                                chs...>>::symbol>>::type>,
                    identity<Result>>
            {
                using next = typename std::conditional_t<
                    is_concatenate_v<C>,
                    identity<typename term_list_helper<
                        typename remove_leading_space_and_comment<typename term<
                            remove_leading_space_and_comment_n<chs...>>::next>::
                            next,
                        type_list_push_back_m<
                            Result,
                            typename term<remove_leading_space_and_comment_n<
                                chs...>>::symbol>>::next>,
                    identity<t_string<C, chs...>>>::type;
            };
            template <typename Result>
            struct term_list_helper<t_string<>, Result>
            {
                using type = Result;
                using next = t_string<>;
            };
            using term_list =
                typename term_list_helper<recursion_start,
                                          typename leading_term::symbol>::type;
            using term_list_next =
                typename term_list_helper<recursion_start,
                                          typename leading_term::symbol>::next;

        public:
            using start  = t_string<chars...>;
            using symbol = term_list; // type_list<pair<flag, value>, ...>
            using next =
                typename remove_leading_space_and_comment<term_list_next>::next;
            static constexpr bool value =
                !std::is_same_v<remove_leading_space_and_comment_n<chars...>,
                                next>;
        };
        template <char... chs>
        using single_definition_t = single_definition<t_string<chs...>>;
        template <char... chs>
        using single_definition_n = typename single_definition_t<chs...>::next;
        template <char... chars>
        struct definitions_list<t_string<chars...>>
        {
        private:
            using leading_definition = single_definition_t<chars...>;
            using recursion_start    = typename leading_definition::next;
            template <typename T, typename Result>
            struct definition_list_helper;
            template <typename Result, char... chs>
            using definition_list_helper_next = typename definition_list_helper<
                typename single_definition<
                    typename remove_leading_space_and_comment<
                        is_definition_separator_n<chs...>>::next>::next,
                type_list_push_back<
                    Result,
                    typename single_definition_t<chs...>::symbol>>::next;
            template <template <typename, auto...> typename T,
                      typename V,
                      auto... vals>
            struct lazy
            {
                using type = T<V, vals...>;
            };
            template <typename Result, char C, char... chs>
            struct definition_list_helper<t_string<C, chs...>, Result>:
                std::conditional_t<
                    is_definition_separator_v<C, chs...>,
                    tavl::lazy_template<
                        definition_list_helper,
                        typename single_definition<
                            typename remove_leading_space_and_comment<
                                is_definition_separator_n<chs...>>::next>::next,
                        type_list_push_back<
                            Result,
                            typename single_definition<
                                typename remove_leading_space_and_comment<
                                    is_definition_separator_n<chs...>>::next>::
                                symbol>>,
                    tavl::lazy_template<identity, Result>>::type
            {
                using next = typename std::conditional_t<
                    is_definition_separator_v<C, chs...>,
                    lazy<definition_list_helper_next, Result, chs...>,
                    identity<t_string<C, chs...>>>::type;
            };
            template <typename Result>
            struct definition_list_helper<t_string<>, Result>
            {
                using type = Result;
                using next = t_string<>;
            };
            using definition_list = typename definition_list_helper<
                recursion_start,
                type_list<typename leading_definition::symbol>>::type;
            using definition_list_next =
                typename remove_leading_space_and_comment<
                    typename definition_list_helper<
                        recursion_start,
                        type_list<typename leading_definition::symbol>>::next>::
                    next;

        public:
            using start = t_string<chars...>;
            using symbol =
                definition_list; //! type_list<single_definition(type_list),
                                 //! ...>
            using next = definition_list_next;
            static constexpr bool value =
                !std::is_same_v<remove_leading_space_and_comment_n<chars...>,
                                next>;
        };
        template <char... chs>
        using definitions_list_t = definitions_list<t_string<chs...>>;
        template <char... chs>
        using definitions_list_n = typename definitions_list_t<chs...>::next;
        template <char... chars>
        struct rule<t_string<chars...>>
        {
        private:
            using rule_start_identifier = typename meta_identifier<
                remove_leading_space_and_comment_n<chars...>>::symbol;
            static constexpr bool rule_defining =
                is_defining<typename remove_leading_space_and_comment<
                    typename meta_identifier<remove_leading_space_and_comment_n<
                        chars...>>::next>::next>::value;
            using definition_start =
                typename is_defining<meta_identifier_n<chars...>>::next;
            using definitions      = definitions_list<definition_start>;
            using definitions_next = typename remove_leading_space_and_comment<
                typename definitions::next>::next;
            static constexpr bool rule_terminator =
                is_terminator<definitions_next>::value;
            template <typename T>
            struct end_of_rule_checker
            {
                static_assert(
                    rule_terminator,
                    "Bad term or missing rule terminator ';' here. See the original type from the Compiler's output('In instantiation of ...' if using g++) for more information.");
                using type = T;
            };

        public:
            using start  = t_string<chars...>;
            using symbol = type_list<rule_start_identifier,
                                     definitions>; //! type_list<t_string,
                                                   //! definitions_list>
            using next = typename end_of_rule_checker<
                typename remove_leading_space_and_comment<
                    typename is_terminator<definitions_next>::next>::next>::
                type;
            static constexpr bool value = rule_defining && rule_terminator;
        };
    } // namespace Impl

    using special_sequence_leading =
        t_string<'\'', '"'>; // string that cannot disappear in normal
                             // terminal strings
    using lexer_rule_indicator = _TSTR("lexer");
    template <char... chars>
    constexpr std::true_type
        start_with_lexer(t_string<'l', 'e', 'x', 'e', 'r', chars...>)
    {
        return {};
    }
    constexpr std::false_type start_with_lexer(...)
    {
        return {};
    }
    template <typename Definition>
    inline constexpr bool is_lexer_rule = std::
        is_same_v<typename type_list_top<Definition>::first_type, Impl::special_sequence_flag>&& decltype(
            start_with_lexer(
                std::declval<
                    typename type_list_top<Definition>::second_type>()))::value;

#define DECL_RULE_S(name, suffix, str) \
    static constexpr const char name##suffix[] = str
#define DECL_RULE(name, str) DECL_RULE_S(name, _rule, str)
} // namespace Compiler
#endif
