// ParserInString -*- C++ -*-

// Copyright (C) 2021  Scott Brown

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

#ifndef PARSER_IN_STRING_RULES_RULES_COMMON
#define PARSER_IN_STRING_RULES_RULES_COMMON
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include <type_traits>
namespace Compiler
{
    namespace Impl
    {
        template <typename T>
        struct definitions_list;
        template <typename T>
        struct rule;
        template <typename T>
        struct remove_leading_space_and_comment;
        struct optional_sequence_flag
        {
            static constexpr int value = 0;
        };
        struct repeated_sequence_flag
        {
            static constexpr int value = 1;
        };
        struct grouped_sequence_flag
        {
            static constexpr int value = 2;
        };
        struct meta_identifier_flag
        {
            static constexpr int value = 3;
        };
        struct terminal_string_flag
        {
            static constexpr int value = 4;
        };
        struct special_sequence_flag
        {
            static constexpr int value = 5;
        };
        struct empty_sequence_flag
        {
            static constexpr int value = 6;
        };
    } // namespace Impl

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
} // namespace Compiler
#endif