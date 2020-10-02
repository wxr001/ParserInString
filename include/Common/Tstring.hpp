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

#ifndef PARSER_IN_STRING_COMMON_TSTRING
#define PARSER_IN_STRING_COMMON_TSTRING

#include "Identity.hpp"
#include "TAVL.hpp"
#include <type_traits>
#include <utility>

namespace Compiler
{
    // Code of t_string originally from StackOverflow
    // https://stackoverflow.com/questions/1826464/c-style-strings-as-template-arguments/28209546#28209546
    template <typename CharT, CharT... chars>
    using t_basic_string = std::integer_sequence<CharT, chars...>;
    template <char... chars>
    using t_string = t_basic_string<char, chars...>;

    template <typename T>
    inline constexpr std::false_type is_t_string(T)
    {
        return std::false_type{};
    }
    template <char... chars>
    inline constexpr std::true_type is_t_string(t_string<chars...>)
    {
        return std::true_type{};
    }
    template <typename T>
    static constexpr bool is_t_string_v =
        decltype(is_t_string(std::declval<T>()))::value;
    template <char... chars>
    constexpr std::integral_constant<int, sizeof...(chars)>
        t_string_size(t_string<chars...>)
    {
        return {};
    }
    template <typename T>
    inline constexpr std::size_t t_string_size_v =
        decltype(t_string_size(std::declval<T>()))::value;
    template <char C, char... chars>
    constexpr t_string<C, chars...> add_one(t_string<chars...>)
    {
        return {};
    }
    template <char... chars_a, char... chars_b>
    constexpr t_string<chars_a..., chars_b...> concat(t_string<chars_a...>,
                                                      t_string<chars_b...>)
    {
        return {};
    }
    template <char... chars_a, char... chars_b>
    constexpr t_string<chars_a..., ' ', chars_b...>
        concat_space(t_string<chars_a...>, t_string<chars_b...>)
    {
        return {};
    }
    template <char... chars_a, char... chars_b>
    constexpr std::true_type start_with(t_string<chars_b..., chars_a...>,
                                        t_string<chars_b...>)
    {
        return {};
    }
    constexpr std::false_type start_with(...)
    {
        return {};
    }
    template <typename T>
    struct merge_space_s;
    template <char... chars>
    struct merge_space_s<t_string<chars...>>
    {
        template <char ch>
        static constexpr bool is_space = (ch == ' ') || (ch == '\t') ||
                                         (ch == '\v') || (ch == '\f');
        template <char...>
        struct remove_prefix
        {
            using type = t_string<>;
        };
        template <char C, char... chs>
        struct remove_prefix<C, chs...>
        {
            template <char... cs>
            struct lazy: public remove_prefix<cs...>
            {};
            using type = typename std::conditional_t<
                is_space<C>,
                lazy<chs...>,
                identity<t_string<C, chs...>>>::type;
        };
        template <char C>
        struct remove_prefix<C>
        {
            using type =
                std::conditional_t<is_space<C>, t_string<>, t_string<C>>;
        };
        template <char...>
        struct save_one_space
        {
            using type = t_string<>;
        };
        template <char C, char... chs>
        struct save_one_space<C, chs...>
        {
            template <char... cs>
            struct lazy:
                public identity<decltype(
                    concat(t_string<' '>(),
                           typename remove_prefix<cs...>::type()))>
            {};
            using type = typename std::conditional_t<
                is_space<C>,
                lazy<chs...>,
                identity<t_string<C, chs...>>>::type;
        };
        template <char C>
        struct save_one_space<C>
        {
            using type = t_string<C>;
        };
        template <typename T>
        struct helper
        {
            using type = t_string<>;
        };
        template <char C, char... chs>
        struct helper<t_string<C, chs...>>
        {
            using type = decltype(
                concat(t_string<C>(),
                       typename helper<
                           typename save_one_space<chs...>::type>::type()));
        };
        template <char C>
        struct helper<t_string<C>>
        {
            using type =
                std::conditional_t<is_space<C>, t_string<>, t_string<C>>;
        };
        template <typename T>
        struct remove_postfix
        {
            using type = t_string<>;
        };
        template <char C, char... chs>
        struct remove_postfix<t_string<C, chs...>>
        {
            template <char ch, char... cs>
            struct lazy:
                public identity<decltype(
                    concat(t_string<ch>(),
                           typename remove_postfix<t_string<cs...>>::type()))>
            {};
            static constexpr bool value = (is_space<C>)&&((is_space<chs>)&&...);
            using type                  = typename std::conditional_t<value,
                                                     identity<t_string<>>,
                                                     lazy<C, chs...>>::type;
        };
        template <char C>
        struct remove_postfix<t_string<C>>
        {
            using type =
                std::conditional_t<is_space<C>, t_string<>, t_string<C>>;
        };
        using type = typename remove_postfix<typename helper<
            typename remove_prefix<chars...>::type>::type>::type;
    };
    template <typename T>
    using merge_space = typename merge_space_s<T>::type;
    template <char...>
    struct remove_space_c
    {
        using type = t_string<>;
    };
    template <char C, char... chs>
    struct remove_space_c<C, chs...>
    {
        template <char ch>
        static constexpr bool is_space = (ch == ' ') || (ch == '\t') ||
                                         (ch == '\v') || (ch == '\f');
        template <char... cs>
        struct lazy: public remove_space_c<cs...>
        {};
        using type =
            typename std::conditional_t<is_space<C>,
                                        lazy<chs...>,
                                        identity<t_string<C, chs...>>>::type;
    };
    template <char C>
    struct remove_space_c<C>
    {
        template <char ch>
        static constexpr bool is_space = (ch == ' ') || (ch == '\t') ||
                                         (ch == '\v') || (ch == '\f');
        using type = std::conditional_t<is_space<C>, t_string<>, t_string<C>>;
    };
    template <typename T>
    struct remove_space_s;
    template <char... chars>
    struct remove_space_s<t_string<chars...>>: remove_space_c<chars...>
    {};
    template <char... chars>
    using remove_space = typename remove_space_c<chars...>::type;
    template <typename T>
    using remove_space_s_t = typename remove_space_s<T>::type;
    template <char... chars>
    struct ignore_one;
    template <char C, char... chars>
    struct ignore_one<C, chars...>
    {
        using type = t_string<chars...>;
    };
    template <char C>
    struct ignore_one<C>
    {
        using type = t_string<>;
    };
    template <>
    struct ignore_one<>
    {
        using type = t_string<>;
    };
    template <typename T>
    struct ignore_one_s;
    template <char... cs>
    struct ignore_one_s<t_string<cs...>>: ignore_one<cs...>
    {};
    template <char... chars>
    using ignore_one_t = typename ignore_one<chars...>::type;
    template <typename T>
    using ignore_one_s_t = typename ignore_one_s<T>::type;
    template <typename T>
    struct first_character_s;
    template <char C, char... chars>
    struct first_character_s<t_string<C, chars...>>
    {
        static constexpr char value = C;
        using type                  = t_string<C>;
    };
    template <>
    struct first_character_s<t_string<>>
    {
        static constexpr char value = 0;
        using type                  = t_string<>;
    };
    template <char... chars>
    using first_character_t =
        typename first_character_s<t_string<chars...>>::type;
    template <char... chars>
    static constexpr char first_character_v =
        first_character_s<t_string<chars...>>::value;
    namespace Impl
    {
        template <int N, int M>
        constexpr char getchar(const char (&c)[M]) noexcept
        {
            if (N < M)
            {
                return c[N];
            }
            else
            {
                return c[M - 1];
            }
        }
        template <typename T, char... C>
        struct remove_zero_s;
        template <char ch, char... Seq, char... C>
        struct remove_zero_s<t_basic_string<char, Seq...>, ch, C...>
        {
        private:
            template <template <typename...> typename T, typename... V>
            struct lazy_helper_t
            {
                using type = T<V...>;
            };
            template <template <typename, auto...> typename T,
                      typename V,
                      auto... chs>
            struct lazy_helper_v
            {
                using type = T<V, chs...>;
            };

        public:
            using type = typename std::conditional_t<
                ch == '\0',
                identity<identity<t_basic_string<char, Seq...>>>,
                lazy_helper_v<remove_zero_s,
                              t_basic_string<char, Seq..., ch>,
                              C...>>::type::type;
        };
        template <char... Seq>
        struct remove_zero_s<t_basic_string<char, Seq...>>
        {
            using type = t_basic_string<char, Seq...>;
        };
        template <char... C>
        using remove_zero_t =
            typename remove_zero_s<t_basic_string<char>, C...>::type;
    } // namespace Impl

// clang-format off
#define TSTR16(n, x)                                        \
    Impl::getchar<0x##n##0>(x), Impl::getchar<0x##n##1>(x), \
    Impl::getchar<0x##n##2>(x), Impl::getchar<0x##n##3>(x), \
    Impl::getchar<0x##n##4>(x), Impl::getchar<0x##n##5>(x), \
    Impl::getchar<0x##n##6>(x), Impl::getchar<0x##n##7>(x), \
    Impl::getchar<0x##n##8>(x), Impl::getchar<0x##n##9>(x), \
    Impl::getchar<0x##n##A>(x), Impl::getchar<0x##n##B>(x), \
    Impl::getchar<0x##n##C>(x), Impl::getchar<0x##n##D>(x), \
    Impl::getchar<0x##n##E>(x), Impl::getchar<0x##n##F>(x)
#define TSTR128(n, x)                 \
    TSTR16(n##0, x), TSTR16(n##1, x), \
    TSTR16(n##2, x), TSTR16(n##3, x), \
    TSTR16(n##4, x), TSTR16(n##5, x), \
    TSTR16(n##6, x), TSTR16(n##7, x)
#define TSTR256(n, x)                 \
    TSTR16(n##0, x), TSTR16(n##1, x), \
    TSTR16(n##2, x), TSTR16(n##3, x), \
    TSTR16(n##4, x), TSTR16(n##5, x), \
    TSTR16(n##6, x), TSTR16(n##7, x), \
    TSTR16(n##8, x), TSTR16(n##9, x), \
    TSTR16(n##A, x), TSTR16(n##B, x), \
    TSTR16(n##C, x), TSTR16(n##D, x), \
    TSTR16(n##E, x), TSTR16(n##F, x)
#define TSTR1024(n, x)                  \
    TSTR256(n##0, x), TSTR256(n##1, x), \
    TSTR256(n##2, x), TSTR128(n##3, x), \
    TSTR16(n##38, x), TSTR16(n##39, x), \
    TSTR16(n##3A, x), TSTR16(n##3B, x), \
    TSTR16(n##3C, x), TSTR16(n##3D, x), \
    TSTR16(n##3E, x), TSTR16(n##3F, x)
#define _TSTR(x) \
    Impl::remove_zero_t<TSTR128(, x)>
    // clang-format on
} // namespace Compiler
namespace InjectedString
{
    // The following code originally from StackOverflow
    // https://stackoverflow.com/questions/1826464/c-style-strings-as-template-arguments#7420711
    // with some modifications
    using Compiler::add_one;
    using Compiler::t_string;

    constexpr int strc(const char* str, int N)
    {
        return str[N];
    }

    constexpr int slen(const char* str)
    {
        return *str ? 1 + slen(str + 1) : 0;
    }

// usage:
// global scope:
// using namespace ImjectedString;
// constexpr const char* XXX = "";
// make_builder(name, XXX);
// in a function:
// builder_name<slen(XXX)>::type
#define make_builder(name, teststr)                                 \
    template <int N, int P = 0>                                     \
    struct builder_##name                                           \
    {                                                               \
        typedef typename builder_##name<N, P + 1>::type      child; \
        typedef decltype(add_one<strc(teststr, P)>(child())) type;  \
    };                                                              \
                                                                    \
    template <int N>                                                \
    struct builder_##name<N, N>                                     \
    {                                                               \
        typedef t_string<strc(teststr, N)> type;                    \
    }
} // namespace InjectedString
#endif
